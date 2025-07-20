#include <Bayesnet.h>
#include <Bayesnode.h>
#include <LWSampler.h>
#include <JointProbabilityTable.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

enum Node { HORARIO_DO_DIA = 0, CHUVA = 1, EVENTO = 2, DIA_SEMANA = 3, TRANSITO = 4 };

void ConfigurarRede(bayonet::Bayesnet& rede) {
    //tudo impacta em transito
    rede.AddEdge(HORARIO_DO_DIA, TRANSITO);
    rede.AddEdge(CHUVA, TRANSITO);
    rede.AddEdge(EVENTO, TRANSITO);
    rede.AddEdge(DIA_SEMANA, TRANSITO);

    //probabilidades baseadas em dados
    //sem pico, pico manhã, pico tarde
    rede[HORARIO_DO_DIA].conditionalTable.SetProbabilities({}, {0.5, 0.3, 0.2}); 
    //não chove, chove
    rede[CHUVA].conditionalTable.SetProbabilities({}, {0.7, 0.3}); 
    //sem evento, com evento
    rede[EVENTO].conditionalTable.SetProbabilities({}, {0.9, 0.1}); 
    //dia útil (5/7), fim de semana/feriado (2/7) + ou _
    rede[DIA_SEMANA].conditionalTable.SetProbabilities({}, {0.714, 0.286}); 

    //probabilidades condicionais para TRANSITO
    //(TRANSITO | HORARIO_DO_DIA, CHUVA, EVENTO, DIA_SEMANA) = {sem trânsito, com trânsito}
    //para HORARIO_DO_DIA = 0 (sem pico), DIA_SEMANA = 0 (dia útil)
    rede[TRANSITO].conditionalTable.SetProbabilities({0, false, false, 0}, {0.8, 0.2});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, false, true, 0},  {0.6, 0.4});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, true, false, 0},  {0.5, 0.5});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, true, true, 0},   {0.3, 0.7});
    //para HORARIO_DO_DIA = 0 (sem pico), DIA_SEMANA = 1 (fim de semana)
    rede[TRANSITO].conditionalTable.SetProbabilities({0, false, false, 1}, {0.85, 0.15});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, false, true, 1},  {0.65, 0.35});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, true, false, 1},  {0.55, 0.45});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, true, true, 1},   {0.35, 0.65});
    //para HORARIO_DO_DIA = 1 (pico manhã), DIA_SEMANA = 0 (dia útil)
    rede[TRANSITO].conditionalTable.SetProbabilities({1, false, false, 0}, {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, false, true, 0},  {0.2, 0.8});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, true, false, 0},  {0.2, 0.8});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, true, true, 0},   {0.1, 0.9});
    //para HORARIO_DO_DIA = 1 (pico manhã), DIA_SEMANA = 1 (fim de semana)
    rede[TRANSITO].conditionalTable.SetProbabilities({1, false, false, 1}, {0.4, 0.6});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, false, true, 1},  {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, true, false, 1},  {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, true, true, 1},   {0.2, 0.8});
    //para HORARIO_DO_DIA = 2 (pico tarde/noite), DIA_SEMANA = 0 (dia útil)
    rede[TRANSITO].conditionalTable.SetProbabilities({2, false, false, 0}, {0.4, 0.6});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, false, true, 0},  {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, true, false, 0},  {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, true, true, 0},   {0.2, 0.8});
    //para HORARIO_DO_DIA = 2 (pico tarde/noite), DIA_SEMANA = 1 (fim de semana)
    rede[TRANSITO].conditionalTable.SetProbabilities({2, false, false, 1}, {0.5, 0.5});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, false, true, 1},  {0.4, 0.6});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, true, false, 1},  {0.4, 0.6});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, true, true, 1},   {0.3, 0.7});
}

int MapearHora(int hora) {
    if (hora >= 6 && hora < 8) return 1;  //pico manhã
    if (hora >= 17 && hora < 19) return 2; //pico tarde/noite
    return 0;  //horario comum
}

void ObterEntradas(int& horaEstado, bool& chovendo, bool& evento, bool& diaSemana) {
    int horaDigitada;
    std::cout << "Informe a hora (0–23): ";
    while (!(std::cin >> horaDigitada) || horaDigitada < 0 || horaDigitada > 23) {
        std::cout << "Hora inválida! Digite um valor entre 0 e 23: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    horaEstado = MapearHora(horaDigitada);

    std::cout << "Está chovendo? (1 = sim, 0 = não): ";
    while (!(std::cin >> chovendo) || (chovendo != 0 && chovendo != 1)) {
        std::cout << "Entrada inválida! Digite 0 ou 1: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "Tem evento na cidade? (1 = sim, 0 = não): ";
    while (!(std::cin >> evento) || (evento != 0 && evento != 1)) {
        std::cout << "Entrada inválida! Digite 0 ou 1: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "É fim de semana? (1 = sim, 0 = dia útil): ";
    while (!(std::cin >> diaSemana) || (diaSemana != 0 && diaSemana != 1)) {
        std::cout << "Entrada inválida! Digite 0 ou 1: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    //rede com 5 nós: HORARIO_DO_DIA (3 estados), CHUVA (2), EVENTO (2), DIA_SEMANA (2), TRANSITO (2)
    bayonet::Bayesnet rede({3, 2, 2, 2, 2});
    ConfigurarRede(rede);

    int horaEstado;
    bool chovendo, evento, diaSemana;
    ObterEntradas(horaEstado, chovendo, evento, diaSemana);

    rede[HORARIO_DO_DIA].SetEvidence(horaEstado);
    rede[CHUVA].SetEvidence(chovendo);
    rede[EVENTO].SetEvidence(evento);
    rede[DIA_SEMANA].SetEvidence(diaSemana);


    bayonet::LWSampler sampler;
    bayonet::JointProbabilityTable tabela = sampler.ReturnJointProbabilityTable(rede, 5000);

    double probSemTransito = tabela.ReturnMarginal(TRANSITO, 0) * 100.0;
    double probComTransito = tabela.ReturnMarginal(TRANSITO, 1) * 100.0;

    std::cout << "\n==== Previsão de Trânsito ====\n";
    std::cout << "Condições:\n";
    std::cout << "- Hora: " << (horaEstado == 0 ? "Fora de pico" : horaEstado == 1 ? "Pico manhã" : "Pico tarde/noite") << "\n";
    std::cout << "- Chuva: " << (chovendo ? "Sim" : "Não") << "\n";
    std::cout << "- Evento: " << (evento ? "Sim" : "Não") << "\n";
    std::cout << "- Dia: " << (diaSemana ? "Fim de semana" : "Dia útil") << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Probabilidade de trânsito: " << probComTransito << "%\n";
    std::cout << "Probabilidade de não ter trânsito: " << probSemTransito << "%\n";

    return 0;
}
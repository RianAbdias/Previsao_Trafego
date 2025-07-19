#include <Bayesnet.h>
#include <Bayesnode.h>
#include <LWSampler.h>
#include <JointProbabilityTable.h>
#include <iostream>
#include <string>

int main() {
    enum node {
        HORA = 0,
        CHUVA = 1,
        EVENTO = 2,
        TRANSITO = 3
    };

    //todos booleanos e impactam em transito
    bayonet::Bayesnet rede({3, 2, 2, 2});

    rede.AddEdge(HORA, TRANSITO);
    rede.AddEdge(CHUVA, TRANSITO);
    rede.AddEdge(EVENTO, TRANSITO);

    //hora (sem horario de pico, horario de pico de manhã, horario de pico tarde/noite)
    rede[HORA].conditionalTable.SetProbabilities({}, {0.5, 0.3, 0.2});

    //chuva (nao chove/chove)
    rede[CHUVA].conditionalTable.SetProbabilities({}, {0.7, 0.3});  

    //evento (tem, nao tem)
    rede[EVENTO].conditionalTable.SetProbabilities({}, {0.9, 0.1}); 

    //transito: P(transito / hora, chuva, evento)
    //com hora 0
    rede[TRANSITO].conditionalTable.SetProbabilities({0, false, false}, {0.8, 0.2});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, false, true},  {0.6, 0.4});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, true, false},  {0.5, 0.5});
    rede[TRANSITO].conditionalTable.SetProbabilities({0, true, true},   {0.3, 0.7});
    //com hora 1
    rede[TRANSITO].conditionalTable.SetProbabilities({1, false, false}, {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, false, true},  {0.2, 0.8});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, true, false},  {0.2, 0.8});
    rede[TRANSITO].conditionalTable.SetProbabilities({1, true, true},   {0.1, 0.9});
    //com hora 2
    rede[TRANSITO].conditionalTable.SetProbabilities({2, false, false}, {0.4, 0.6});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, false, true},  {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, true, false},  {0.3, 0.7});
    rede[TRANSITO].conditionalTable.SetProbabilities({2, true, true},   {0.2, 0.8});

    int horaDigitada;
    bool chovendo, evento;

    std::cout << "Informe a hora (0–23): ";
    std::cin >> horaDigitada;

    int horaEstado = 0;
    if ((horaDigitada >= 6 && horaDigitada <= 7)) {
        horaEstado = 1;
    } else if ((horaDigitada >= 12 && horaDigitada <= 13) || (horaDigitada >= 18 && horaDigitada <= 19)) {
        horaEstado = 2;
    }

    std::cout << "Está chovendo? (1 = sim, 0 = não): ";
    std::cin >> chovendo;

    std::cout << "Tem evento na cidade? (1 = sim, 0 = não): ";
    std::cin >> evento;

    rede[HORA].SetEvidence(horaEstado);
    rede[CHUVA].SetEvidence(chovendo);
    rede[EVENTO].SetEvidence(evento);

    bayonet::LWSampler sampler;
    bayonet::JointProbabilityTable tabela = sampler.ReturnJointProbabilityTable(rede, 50000);

    std::cout << "\n==== Resultado da previsão de trânsito ====" << std::endl;
    tabela.PrintMarginal(TRANSITO);

    double probSemTransito = tabela.ReturnMarginal(TRANSITO, 0) * 100.0;
    double probComTransito = tabela.ReturnMarginal(TRANSITO, 1) * 100.0;

    std::cout << std::fixed;
    std::cout.precision(2);
    std::cout << "Há " << probComTransito << "% de chance de ter trânsito e "<< probSemTransito << "% de chance de não ter trânsito." << std::endl;

    std::cout << "\n" << std::endl;

    return 0;
}

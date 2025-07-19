#include <Bayesnet.h>
#include <Bayesnode.h>
#include <BeliefPropagator.h>
#include <GibbsSampler.h>
#include <ConditionalProbabilityTable.h>
#include <JointProbabilityTable.h>
#include <LWSampler.h>
#include <MarginalProbabilityTable.h>
#include <MaximumLikelihoodLearning.h>
#include <RejectionSampler.h>
#include <iostream>
#include <string>

int main() {
 //This enum is a way to deal with the node in a more confortable way.
 //Instead of using numbers we are going to use words.
 enum node{
  CLOUDY=0,
  SPRINKLER=1,
  RAIN=2,
  GRASS=3
 };

  //It creates a net with 4 nodes, each node can assume 2 states
 bayonet::Bayesnet myNet({2, 2, 2, 2});

 //It defines the number of iteration to use for the sampler
 //and the sampler we are going to use.
 unsigned int iterations = 50000;
 bayonet::LWSampler myLWSampler;
 //bayonet::GibbsSampler myGibbsSampler; //A Gibbs sampler can be used too


 //Setting the Bayesian network.
 //There are 4 boolean nodes: CLOUDY, SPRINKLER, RAIN, GRASS
 //The structure is like a diamond.
 myNet.AddEdge(CLOUDY, SPRINKLER);
 myNet.AddEdge(CLOUDY, RAIN);
 myNet.AddEdge(SPRINKLER, GRASS);
 myNet.AddEdge(RAIN, GRASS);

 //Setting the Conditional tables for each node.
 //The conditional table contains the probabilities associated with each
 //state of each parent of the current node.
 myNet[CLOUDY].conditionalTable.SetProbabilities({}, {0.50, 0.50});

 myNet[SPRINKLER].conditionalTable.SetProbabilities({true}, {0.90, 0.10});
 myNet[SPRINKLER].conditionalTable.SetProbabilities({false}, {0.50, 0.50});

 myNet[RAIN].conditionalTable.SetProbabilities({true}, {0.20, 0.80});
 myNet[RAIN].conditionalTable.SetProbabilities({false}, {0.80, 0.20});

 myNet[GRASS].conditionalTable.SetProbabilities({true,true}, {0.01, 0.99});
 myNet[GRASS].conditionalTable.SetProbabilities({true,false}, {0.10, 0.90});
 myNet[GRASS].conditionalTable.SetProbabilities({false,true}, {0.10, 0.90});
 myNet[GRASS].conditionalTable.SetProbabilities({false,false}, {1.0, 0.0});

 //The output of the sampler is a JointTable, that is a useful
 //way to deal with joint probabilities. Here the inference is made.
 bayonet::JointProbabilityTable myJointTable = myLWSampler.ReturnJointProbabilityTable(myNet,iterations);

 //Here we are going to print the result of the sampling
 std::cout << std::endl << "==== Initial conditions without evidences ====" << std::endl;
 std::cout << std::endl << "-------------- CLOUDY --------------" << std::endl;
 myJointTable.PrintMarginal(CLOUDY);
 std::cout << std::endl << "------------ SPRINKLER ------------" << std::endl;
 myJointTable.PrintMarginal( SPRINKLER);
 std::cout << std::endl << "-------------- RAIN --------------" << std::endl;
 myJointTable.PrintMarginal(RAIN);
 std::cout << std::endl << "-------------- GRASS -------------" << std::endl;
 myJointTable.PrintMarginal(GRASS);
 std::cout << std::endl;

 //In the following sections we are going to play with the nodes,
 //setting up different inference sates and looking for the results.
 myNet[GRASS].SetEvidence(true);
 myJointTable = myLWSampler.ReturnJointProbabilityTable(myNet,iterations);

 std::cout << std::endl << "========= The GRASS is wet =========" << std::endl;
 std::cout << std::endl << "-------------- CLOUDY --------------" << std::endl;
 myJointTable.PrintMarginal(CLOUDY);
 std::cout << std::endl << "------------ SPRINKLER ------------" << std::endl;
 myJointTable.PrintMarginal( SPRINKLER);
 std::cout << std::endl << "-------------- RAIN --------------" << std::endl;
 myJointTable.PrintMarginal(RAIN);
 std::cout << std::endl << "-------------- GRASS -------------" << std::endl;
 myJointTable.PrintMarginal(GRASS);
 std::cout << std::endl;

 myNet[GRASS].SetEvidence(-1);
 myNet[CLOUDY].SetEvidence(true);
 myJointTable = myLWSampler.ReturnJointProbabilityTable(myNet,iterations);

 std::cout << std::endl << "====== The weather is CLOUDY  ======" << std::endl;
 std::cout << std::endl << "-------------- CLOUDY --------------" << std::endl;
 myJointTable.PrintMarginal(CLOUDY);
 std::cout << std::endl << "------------ SPRINKLER ------------" << std::endl;
 myJointTable.PrintMarginal( SPRINKLER);
 std::cout << std::endl << "-------------- RAIN --------------" << std::endl;
 myJointTable.PrintMarginal(RAIN);
 std::cout << std::endl << "-------------- GRASS -------------" << std::endl;
 myJointTable.PrintMarginal(GRASS);
 std::cout << std::endl;

 return 0;
}
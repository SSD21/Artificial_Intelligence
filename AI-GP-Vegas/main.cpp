/*
AI Group Project
Group Name: Vegas
Class: COSC 4368
Students:
Joann Pham 1589075
Max Broekhuis 1808005
Pablo Bribiesca R.: 1597268
Samita Damani: 1980918
Muaz Bourhani: 1869692
*/

#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include <iomanip>  

using namespace std;

////World Objects

//Cell - makes up the world.
class cell {
public:
  int blocks;       //How many blocks are in this cell.
  bool terminal;    //If this is a terminal cell.
  bool producer;    //If this is a producer cell.
  bool options[4];  //Options available [[N,S,W,E]].
  float opVals[6];  //Values of taking particular actions in this state.  These are the elements of the Q-table.

  //Constructor
  cell() {
    blocks = 0;
    terminal = false;
    producer = false;
    for (int i = 0; i < 6; i++) {
      opVals[i] = 0;
    }
  };

  //Make this cell a producer.
  void setProducer() {
    this->blocks = 8;
    this->producer = true;
  };

  //Make this cell a terminal.
  void setTerminal() {
    this->blocks = 0;
    this->terminal = true;
  };

  //Determine which directions the actor may move in this cell.
  void setDirectionalOptions(int row, int col, cell** worldMatrix) {
    for (int i = 0; i < 4; i++) {
      options[i] = true;
    }

    //Determine moving in x directions
    if (col == 0)
      options[2] = false; //Can't move West
    else if (col == 4)
      options[3] = false; //Can't move East

    //Determine moving in y directions
    if (row == 0)
      options[0] = false; //Can't move North
    else if (row == 4)
      options[1] = false; //Can't move South
  };

  //Update the q-value for the action taken.
  void updateVal(float alpha, float discountFactor, int actionTaken, int cellPos[2], cell** worldMatrix, bool hasObject, bool sarsa=false, int prevPosY = 0, int prevPosX = 0, int prevAction = 0) {
    float calVal;  //Value used to hold old value, set to Q(a,s)
    float funP2;  //Variable used to hold alpha*[R(s', a,s) + y*maxa(Q(a's'))] or alpha*[R(s', a,s) + y*Q(A'S')] for SARSA
    int newCellPos[2]; //Variable holding position of next cell

    //Calculate rewards.
    if (actionTaken < 4) //Negative reward for moving.
      funP2 = -1;
    else
      funP2 = 13; //Positive reward for picking up/placing block.

    //Update for Q-learning
    if(!sarsa) {
      calVal = opVals[actionTaken];
      calVal *= (1 - alpha); //first term = Q(a,s)*(1-alpha)
      //Calculate y*maxa(Q(a's'))
      //Find next cell
      if (actionTaken == 0) {  //Moved North
        newCellPos[0] = cellPos[0] - 1;
        newCellPos[1] = cellPos[1];
      }
      else if (actionTaken == 1) { //Moved South
        newCellPos[0] = cellPos[0] + 1;
        newCellPos[1] = cellPos[1];
      }
      else if (actionTaken == 2) { //Moved West
        newCellPos[0] = cellPos[0];
        newCellPos[1] = cellPos[1] - 1;
      }
      else if (actionTaken == 3) { //Moved East
        newCellPos[0] = cellPos[0];
        newCellPos[1] = cellPos[1] + 1;
      }
      else { //Didn't move, pickup/dropoff
        newCellPos[0] = cellPos[0];
        newCellPos[1] = cellPos[1];
      }

      //Calculate utility of next cell
      cell nextCell = worldMatrix[newCellPos[0]][newCellPos[1]];
      float max = -1;

      for (int i = 0; i < 6; i++) {
        if (nextCell.opVals[i] > max) {
          if (i < 4 || (i == 4 && nextCell.producer && !hasObject && nextCell.blocks > 0) || (i == 5 && nextCell.terminal && hasObject && nextCell.blocks < 4))
            max = nextCell.opVals[i];
        }
      }

      funP2 += discountFactor * max;
      calVal += alpha * funP2;

      this->opVals[actionTaken] = calVal;
    } 

    //Calculate sarsa change.  NOTE:  This changes the PREVIOUS state, not this one.
    else if(sarsa) {
      calVal = worldMatrix[prevPosY][prevPosX].opVals[prevAction]; //Q(a,s)
      //calculate funP2 (alpha[R(s)+y*Q(a's')-Q(a,s)])
      //y*Q(a's') (add this action's value)
      funP2 += discountFactor * worldMatrix[cellPos[0]][cellPos[1]].opVals[actionTaken];

      //-Q(a,s)
      funP2 -= calVal;

      calVal += alpha * funP2;

      worldMatrix[prevPosY][prevPosX].opVals[prevAction] = calVal;
    }
  }; //End of updateVal
};

//Set producer and terminal cells to initial state
cell** setPDCells(cell** worldMatrix) {
  cout << "Setting world pickup/dropoff cells to initial state." << endl;
  //Setup function variables
  short colP[2] = { 1,4 };
  short rowP[2] = { 3,2 };
  short colD[4] = { 0,4,2,4 };
  short rowD[4] = { 0,0,2,4 };

  //Create producer and terminal cells
  for (int i = 0; i < 4; i++) {
    worldMatrix[rowD[i]][colD[i]].setTerminal();
  }
  for (int i = 0; i < 2; i++) {
    worldMatrix[rowP[i]][colP[i]].setProducer();
  }

  return worldMatrix;
}

//Creates new world with set pickup/dropoff cells
cell** defineWorldStatic() {
  //Setup 5x5 worldMatrix
  cell** worldMatrix = new cell * [5];
  for (int i = 0; i < 5; i++) {
    worldMatrix[i] = new cell[5];
    for (int j = 0; j < 5; j++) {
      worldMatrix[i][j].setDirectionalOptions(i, j, worldMatrix); //Set which options are available for each cell
    }
  }

  //Set producer and terminal cells
  worldMatrix = setPDCells(worldMatrix);

  return worldMatrix;
}

//Prints formated Q-Matrix
void printLine() {
  string line = "    ";
  for (int length = 0; length < 52; length++) {
    line.append("-");
  }
  cout << "\n";
}

void printWorld(cell** worldMatrix) {
  char direction[] = { 'N', 'S', 'W', 'E', 'P', 'D' };

  for (int i = 0; i < 5; i++) {
    for (int k = 0; k < 6; k++) {
      stringstream stream;
      stream << direction[k] << ":  | ";
      for (int j = 0; j < 5; j++) {
        //stream << fixed << setprecision(3) << worldMatrix[i][j].opVals[k];
        if (worldMatrix[i][j].opVals[k] == 0) {
          stream << setw(8) << '-' << "  |  ";
        }
        else {
          stream << setw(8) << fixed << setprecision(3) << worldMatrix[i][j].opVals[k] << "  |  ";
        }
        //string s = stream.str();
      }
      cout << stream.str() << "\n";
    }
    printLine();
  }
}

////Actor - object which moves around the world.
class actor {
public:
  int pos[2];
  bool hasObject;
  int movedCounter; //How many blocks were moved so far
  int stepCounter;  //What step the program is on
  int reachedTerminal; //Number of times the terminal state is reached
  bool changed = false; //Variables for experiment 4. Whether or not producers have been changed from original
  int prevAction; //Previous action.  Only used with SARSA
  int prevPos[2]; //Previous position.  Only used with SARSA

  //Constructor
  actor() {
    pos[0] = 4; //agent starts at 4,0 always (5,1) based on ppt
    pos[1] = 0;
    hasObject = false;
    movedCounter = 0;
    stepCounter = 0;
    reachedTerminal = 0;
    cout << "Actor starting at (5,1)" << endl;
  };

  //Set actor equal to existing one.
  actor& operator= (const actor& orig) {
    pos[0] = orig.pos[0];
    pos[1] = orig.pos[1];
    hasObject = orig.hasObject;
    movedCounter = orig.movedCounter;

    return *this;
  };

  //Determine which is the best action to take.
  int getBestOption(cell** worldMatrix) {
    //If pickup/dropoff available, choose that.
    if (hasObject && worldMatrix[pos[0]][pos[1]].terminal && worldMatrix[pos[0]][pos[1]].blocks < 4)
      return 5; 
    else if (!hasObject && worldMatrix[pos[0]][pos[1]].producer && worldMatrix[pos[0]][pos[1]].blocks > 0)
      return 4;

    //Choose directional movement
    else {
      float maxVal = -1000; //Arbitrarily low value
      int maxPos = -1;
      for (int i = 0; i < 4; i++) {
        //If the action being looked at is available and better, choose it.
        if (worldMatrix[pos[0]][pos[1]].opVals[i] > maxVal&& worldMatrix[pos[0]][pos[1]].options[i]) {
          maxVal = worldMatrix[pos[0]][pos[1]].opVals[i];
          maxPos = i;
        }
        //If the action being taken is available and is equal to another, choose randomly between it and the other.
        else if (worldMatrix[pos[0]][pos[1]].opVals[i] == maxVal && worldMatrix[pos[0]][pos[1]].options[i]) {
          if (rand() % 2 == 1) {
            maxPos = i;
          }
        }
      }
      //If no direction was taken, problems occurred.
      if (maxPos == -1) {
        cell curCell = worldMatrix[pos[0]][pos[1]];
        cout << "STOP! NO DIRECTION TAKEN";
        exit(0);
      }

      return maxPos;
    }
  } //End of getBestOption

  //Have the actor take an action.
  void moveGivenOption(int option, cell** worldMatrix, bool expt4 = false) {
    //SARSA whether or not the agent is a future agent from SARSA method

    //Take a block from a producer cell
    if (option == 4) {
      worldMatrix[pos[0]][pos[1]].blocks -= 1;
      hasObject = true;
      cout << stepCounter << ": Picked up at (" << pos[0] + 1 << "," << pos[1] + 1 << "). " << worldMatrix[pos[0]][pos[1]].blocks << " remaining." << endl;
    }

    //Deposit block at terminal cell
    else if (option == 5) { 
      worldMatrix[pos[0]][pos[1]].blocks += 1;
      hasObject = false;
      movedCounter++;
      cout << stepCounter << ": Deposited at (" << pos[0] + 1 << "," << pos[1] + 1 << "). " << worldMatrix[pos[0]][pos[1]].blocks << " deposited. (" << movedCounter << " total)" << endl;


      //If 16 blocks were moved, this run is completed. Start again.
      if (movedCounter == 16) {
        cout << "Terminal state reached. 16 blocks moved." << endl << endl;
        reachedTerminal++;  //Increment the number of terminal states reached.
        if (!(reachedTerminal == 6 && expt4)) {
          stepCounter = 0;  //Reset the step counter within the run..
          movedCounter = 0; //Reset the moved counter within the run.
          changed = false; //Resetting the agent.

          cout << "Q-table after " << reachedTerminal << " goal state(s) reached:" << endl;
          printWorld(worldMatrix);

          //Reset the producer and terminal cells.
          worldMatrix = setPDCells(worldMatrix);

          //Reset the actor's position
          pos[0] = 4;
          pos[1] = 0;
          cout << "Actor starting at (5,1)" << endl;
        }
      }
    }

    //Move in a direction.
    else {
      // Move North
      if (option == 0) {
        pos[0] = pos[0] - 1;
        pos[1] = pos[1];
      }

      //Move South
      else if (option == 1) {
        pos[0] = pos[0] + 1;
        pos[1] = pos[1];
      }

      //Move West
      else if (option == 2) { 
        pos[0] = pos[0];
        pos[1] = pos[1] - 1;
      }

      //Move East
      else if (option == 3) { //moved east
        pos[0] = pos[0];
        pos[1] = pos[1] + 1;
      }

      //An error occurred.  Stop the program.
      else {
        cout << "No movement taken." << endl;
        exit(0);
      }
    }
  }; //End of moveGivenOption


  //Function running experiments with the PRandom policy.
  void movePRandom(float alpha, float discountFactor, cell** worldMatrix, int stop = 5500) {
    cout << "\nStarting PRANDOM search." << endl;
    int option; //Option which it will take 

    //Run until it is meant to stop.
    for (int i = 0; i < stop; i++) {
      stepCounter++;

      //Get the option used in PGreedy.
      option = getBestOption(worldMatrix);

      //If PGreedy says to move, do so randomly instead.
      if (option < 4) {
        do {
          option = rand() % 4;
        } while (!worldMatrix[pos[0]][pos[1]].options[option]);
      }

      //Update cell Q-value.
      worldMatrix[pos[0]][pos[1]].updateVal(alpha, discountFactor, option, pos, worldMatrix, hasObject);

      //Move the actor.
      moveGivenOption(option, worldMatrix);
    }

    cout << "\nPRANDOM search end. Goal state reached " << reachedTerminal << " times." << endl;

  }; // End of movePRandom

  //Function running experiments with the PGreedy policy.
  void movePGreedy(float alpha, float discountFactor, cell** worldMatrix) {
    cout << "\nStarting PGREEDY search." << endl;
    int option; //Option which it will take 

    //Run until it is meant to stop (always at 5500 steps).
    for (int i = 0; i < 5500; i++) {
      stepCounter++;

      //Get the best option (core of PGreedy)
      option = getBestOption(worldMatrix);

      //Update cell Q-value
      worldMatrix[pos[0]][pos[1]].updateVal(alpha, discountFactor, option, pos, worldMatrix, hasObject);

      //Move the actor.
      moveGivenOption(option, worldMatrix);
    }

    cout << "\nPGREEDY search end. Goal state reached " << reachedTerminal << " times." << endl;

  }; // End of movePGreedy

  //Function running experiments with the PExploit policy.
  void movePExploit(float alpha, float discountFactor, cell** worldMatrix, bool expt4 = false, bool sarsa = false) {
    if(sarsa)
      cout << endl << "RUNNING WITH SARSA" << endl;
    cout << "\nStarting PEXPLOIT search." << endl;
    int option; //Option which it will take 



    //Run until meant to stop (usually 5500).
    for (int i = 0; i < 5500; i++) {

      //If running experiment 4, change producer and terminal cells after the third terminal state is reached.
      //Setting new producers to (3,1) and (1,3); numbers diff bc array numbering starts at 0
      if (reachedTerminal == 3 && expt4 && !changed) {
        cout << "Producers being changed" << endl;
        changed = true;

        //Set new producers.
        worldMatrix[2][0].producer = true;
        worldMatrix[2][0].blocks = worldMatrix[3][1].blocks;
        worldMatrix[0][2].producer = true;
        worldMatrix[0][2].blocks = worldMatrix[2][4].blocks;

        //Remove old producers
        worldMatrix[2][4].producer = false;
        worldMatrix[2][4].blocks = 0;
        worldMatrix[3][1].producer = false;
        worldMatrix[3][1].blocks = 0;
      }

      //If terminal state 6 is reached, the experiment is over.
      if (reachedTerminal == 6 && expt4) {
        return;
      }

      stepCounter++;

      //Choose the best option as per PGreedy
      option = getBestOption(worldMatrix);

      //If moving, check if you are meant to do so randomly
      if (option < 4) {
        if (rand() % 10 < 2) { //20% probability to choose random operator
          do {
            option = rand() % 4;
          } while (!worldMatrix[pos[0]][pos[1]].options[option]);
        }
      }

      //Update cell Q-value
      if(!sarsa)  //Update via Q-learning
        worldMatrix[pos[0]][pos[1]].updateVal(alpha, discountFactor, option, pos, worldMatrix, hasObject);
      else {  //Update via SARSA
        if(i>0)
          worldMatrix[pos[0]][pos[1]].updateVal(alpha, discountFactor, option, pos, worldMatrix, hasObject, true, prevPos[0], prevPos[1], prevAction);
        this->prevAction = option;  //Set the action taken to be the "previous" action for the next round.
        this->prevPos[0] = this->pos[0];
        this->prevPos[1] = this->pos[1];
      }

      //Move the actor
      moveGivenOption(option, worldMatrix, expt4);

      //Don't end experiment 4 after a certain number of steps.  Never increment i.
      if(expt4)
        i--;
    }

    cout << "\nPEXPLOIT search end. Goal state reached " << reachedTerminal << " times." << endl;

  }; // End of movePExploit
};

//Functions to call experiments
bool Expt1(char part) {
  if (part == 'a' || part == 'A') {
    cout << "------------- Starting Experiment 1a: PRANDOM for 6000 steps --------------" << endl;
    srand(time(0));
    cell** worldMatrix1a = defineWorldStatic();
    actor a1;
    a1.movePRandom(0.3, 0.5, worldMatrix1a, 6000);

    cout << "\nExpt 1a reached goal state " << a1.reachedTerminal << " times.\nFinal Q-table:" << endl;
    printWorld(worldMatrix1a);
  }
  else if (part == 'b' || part == 'B') {
    cout << "------------- Starting Experiment 1b: PGREEDY for 5500 steps --------------" << endl;
    srand(time(0));
    cell** worldMatrix1b = defineWorldStatic();
    actor b1;
    b1.movePRandom(0.3, 0.5, worldMatrix1b, 500);
    b1.movePGreedy(0.3, 0.5, worldMatrix1b);

    cout << "\nExpt 1b reached goal state " << b1.reachedTerminal << " times.\nFinal Q-table:" << endl;
    printWorld(worldMatrix1b);
  }
  else if (part == 'c' || part == 'C') {
    cout << "------------ Starting Experiment 1c: PEXPLOIT for 5500 steps --------------" << endl;
    srand(time(0));
    cell** worldMatrix1c = defineWorldStatic();
    actor c1;
    c1.movePRandom(0.3, 0.5, worldMatrix1c, 500);
    c1.movePExploit(0.3, 0.5, worldMatrix1c);

    cout << "\nExpt 1c reached goal state " << c1.reachedTerminal << " times.\nFinal Q-table:" << endl;
    printWorld(worldMatrix1c);
  }
  else {
    return false;
  }

  cout << "End of experiment 1" << part << ". ------------------------------" << endl << endl;
  return true;
}

void Expt2() {
  cout << "------------- Starting Experiment 2: SARSA for 6000 steps --------------" << endl;
  srand(time(0));
  cell** worldMatrix2 = defineWorldStatic();
  actor a2;
  a2.movePExploit(0.3, 0.5, worldMatrix2, false, true);

  cout << "\nExpt 2 reached goal state " << a2.reachedTerminal << " times.\nFinal Q-table:" << endl;
  printWorld(worldMatrix2);
        
  cout << "End of experiment 2. ------------------------------" << endl << endl;
}

void Expt3() {
  cout << "------ Starting Experiment 3: PEXPLOIT with Different Learning Rate ------" << endl;

  cout << "--------------------- ALPHA = 0.3 ---------------------" << endl;
  srand(time(0));
  cell** worldMatrix3a = defineWorldStatic();
  actor a1;
  a1.movePRandom(0.3, 0.5, worldMatrix3a, 500);
  a1.movePExploit(0.3, 0.5, worldMatrix3a);

  cout << "\nExpt 3 (alpha=0.3) reached goal state " << a1.reachedTerminal << " times.\nFinal Q-table:" << endl;
  printWorld(worldMatrix3a);

  cout << "-------------------- ALPHA = 0.15 ---------------------" << endl;
  srand(time(0));
  cell** worldMatrix3b = defineWorldStatic();
  actor b1;
  b1.movePRandom(0.15, 0.5, worldMatrix3b, 500);
  b1.movePExploit(0.15, 0.5, worldMatrix3b);

  cout << "\nExpt 3 (alpha=0.15) reached goal state " << b1.reachedTerminal << " times.\nFinal Q-table:" << endl;
  printWorld(worldMatrix3b);
        
  cout << "-------------------- ALPHA = 0.45 ---------------------" << endl;
  srand(time(0));
  cell** worldMatrix3c = defineWorldStatic();
  actor c1;
  c1.movePRandom(0.45, 0.5, worldMatrix3c, 500);
  c1.movePExploit(0.45, 0.5, worldMatrix3c);

  cout << "\nExpt 3 (alpha=0.45) reached goal state " << c1.reachedTerminal << " times.\nFinal Q-table:" << endl;
  printWorld(worldMatrix3c);
        
  cout << "End of experiment 3. ------------------------------" << endl << endl;
}

void Expt4() {
  cout << "------ Starting Experiment 4: PEXPLOIT with Changing Pickup Location ------" << endl;

  srand(time(0));
  cell** worldMatrix4 = defineWorldStatic();
  actor a4;
  a4.movePRandom(0.3, 0.5, worldMatrix4, 500);
  a4.movePExploit(0.3, 0.5, worldMatrix4, true);

  cout << "\nExpt 4 reached goal state " << a4.reachedTerminal << " times.\nFinal Q-table:" << endl;
  printWorld(worldMatrix4);
        
  cout << "End of experiment 4. ------------------------------" << endl << endl;
}

int main() {
  int expt;

  do {
    cout << "Which experiment would you like to run? (5 to exit) ";
    cin >> expt;

    if (expt == 1) {
      char part1;
      bool run; //whether or not expt 1 ran

      cout << "Which part of experiment 1 would you like to run? (a, b, c) ";
      cin >> part1;

      run = Expt1(part1);
      if (!run)
        cout << "Invalid part of experiment 1. Experiment 1 did not run." << endl << endl;
    }
    else if (expt == 2) {
      Expt2();
    }
    else if (expt == 3) {
      Expt3();
    }
    else if (expt == 4) {
      Expt4();
    }
    else if (expt == 5) {
      cout << "All experiments done. Exiting." << endl;
      break;
    }
    else {
      cout << "Invalid experiment chosen." << endl;
    }
  } while (expt != 5);

  return 0;
}
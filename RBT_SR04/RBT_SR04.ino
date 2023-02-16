#include "SR04.h"  // Provides functionality to distance sensor
#include "pitches.h"  // Defines note names

#define TRIG_PIN 4
#define ECHO_PIN 3

int highMelody[] = { NOTE_C7, NOTE_D7, NOTE_E7, NOTE_F7, NOTE_G7, NOTE_A7, NOTE_B7 };  // notes for Red Nodes
int lowMelody[] = { NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3 };  // notes for Black Nodes

int duration = 100;                                                                // 100 miliseconds
int delayTime = 100;                                                               // 100 miliseconds

SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long a;  // Distance measured by sensor

// Used by Node class in RBT, defined colour for simplicity of variable names
enum colour {
  red = 1,
  black = 0
};

// Basic Node class, used in RBT
class Node {
private:
  int key, value;
  colour color;
  Node* left;
  Node* right;

public:
  // Constructor
  Node(int k, int v, colour c, Node* l, Node* r) {
    key = k;
    value = v;
    color = c;
    left = l;
    right = r;
  }

  // Destructor
  ~Node() {
    left = nullptr;
    right = nullptr;

    delete left;
    delete right;
  }

  // Getters
  int getKey() {
    return key;
  }

  int getValue() {
    return value;
  }

  colour getColor() {
    return color;
  }

  Node* getLeftNode() {
    return left;
  }

  Node* getRightNode() {
    return right;
  }

  // Setters
  void setKey(int k) {
    key = k;
  }

  void setValue(int v) {
    value = v;
  }

  void setColor(colour c) {
    color = c;
  }

  void setLeftNode(Node* l) {
    left = l;
  }

  void setRightNode(Node* r) {
    right = r;
  }

  // Helper function
  bool isRed() {
    return color == colour::red;
  }
};

// Red-Black Tree Class, used to store and play notes
class RBT {
private:
  Node* root;  // Root node in RBT
  int privateKeyCount;  // Number of nodes in RBT - used to generate keys

  // Uses recursion to add a note to the tree
  Node* put(Node* currentNode, int key, int value) {

    // The root of the tree is null, so the node being put becomes the root
    if (currentNode == nullptr) {
      return new Node(key, value, colour::red, nullptr, nullptr);
    }

    bool compare = key < currentNode->getKey() ? true : false;

    // Key of node to be added is less than key of the root, so it will be
    // to the left of the root, call recursive put
    if (compare) {
      currentNode->setLeftNode(put(currentNode->getLeftNode(), key, value));
    }
    // Key of node to be added is greater than key of the root, so it will be
    // to the right of the root, call recursive put
    else {
      currentNode->setRightNode(put(currentNode->getRightNode(), key, value));
    }

    // We never need to worry about Key being equal because it is incremented internally

    // Now we need to check if the tree needs to be re-balanced

    // Checks if it is right-leaning, if so rotates left
    if (currentNode->getRightNode() != nullptr) {  // Mmmm tasty tasty seg fault

      // There is a red right node, and no left node
      if (currentNode->getLeftNode() == nullptr && currentNode->getRightNode()->isRed()) {
        currentNode = rotateLeft(currentNode);
      } else if (currentNode->getLeftNode() != nullptr) {  // There is a left node
        // Check if only the right node is red
        if (currentNode->getRightNode()->isRed() && !currentNode->getLeftNode()->isRed()) {
          currentNode = rotateLeft(currentNode);
        }
      }
    }

    // Checks if there are two red left nodes (one as the child of another),
    // and if so, rotates right
    if (currentNode->getLeftNode() != nullptr && currentNode->getLeftNode()->getLeftNode() != nullptr) {
      if (currentNode->getLeftNode()->isRed() && currentNode->getLeftNode()->getLeftNode()->isRed()) {
        currentNode = rotateRight(currentNode);
      }
    }

    // Checks if both the left and right child are red, and if so flips the colors
    if (currentNode->getRightNode() != nullptr && currentNode->getLeftNode() != nullptr) {
      if (currentNode->getLeftNode()->isRed() && currentNode->getRightNode()->isRed()) {
        flipColors(currentNode);
      }
    }

    return currentNode;
  }

  // Rotates left, fixing the problem of having red right children
  Node* rotateLeft(Node* currentNode) {
    Node* temp = currentNode->getRightNode();
    currentNode->setRightNode(temp->getLeftNode());
    temp->setLeftNode(currentNode);
    temp->setColor(currentNode->getColor());
    currentNode->setColor(colour::red);
    return temp;
  }

  // Rotates right, fixing the problem of a red left child with its own
  // red left child
  Node* rotateRight(Node* currentNode) {
    Node* temp = currentNode->getLeftNode();
    currentNode->setLeftNode(temp->getRightNode());
    temp->setRightNode(currentNode);
    temp->setColor(currentNode->getColor());
    currentNode->setColor(colour::red);
    return temp;
  }

  // Flips the colors such that the given node is made red
  // and its two children are made black
  void flipColors(Node* currentNode) {
    currentNode->setColor(colour::red);
    currentNode->getLeftNode()->setColor(colour::black);
    currentNode->getRightNode()->setColor(colour::black);
  }

  // Plays an individual note
  void playNote(int value, colour color) {
    if(color == colour::red) {
      tone(11, highMelody[value], duration);
    } else {
      tone(11, lowMelody[value], duration);
    }
    delay(delayTime * 2);
  }

  // Uses recursion to perform an inorder traversal, playing notes in tree
  void playTree(Node* current) {
    if (current->getLeftNode() != nullptr) {
      playTree(current->getLeftNode());
    }

    playNote(current->getValue(), current->getColor());

    if (current->getRightNode() != nullptr) {
      playTree(current->getRightNode());
    }
  }

public:
  // Constructor
  RBT() {
    root = nullptr;
    privateKeyCount = 0;
  }

  // Adds notes to the tree, calling private recursive function
  // Also ensures that root node stays black
  void put(int value) {
    root = put(root, privateKeyCount++, value);
    root->setColor(colour::black);
    playTree();
  }

  // Plays notes of the tree by calling private recursive function
  void playTree() {
    playTree(root);
    Serial.println("Finished playing RBT\n");
  }
};



RBT* theRBT = new RBT();  // Red-Black Tree holding notes of our current melody
int numNodes = 0;  //The number of nodes currently in the RBT

void setup() {
  Serial.begin(9600);  //Initialization of Serial Port
  delay(1000);
}


void loop() {
  a = sr04.Distance();

  theRBT->put(a % 7);
  numNodes++;

  delay(delayTime * 10);

  if(numNodes > 15) {
    Serial.println("Don't want to use up too much memory! Resetting RBT\n");
    delay(delayTime * 50);
    theRBT = new RBT();  // reset RBT
    numNodes = 0;
  }
  
}

#ifndef TREE_H
#define TREE_H

#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>

using namespace std;

struct Node {
  string    element;
  Node*     parent;
  vector<Node*> children;

  Node(const string& elem, Node* p = nullptr)
    : element(elem), parent(p) {}
};


class Tree {
public:
  class Position {
    Node* v;
    friend class Tree;
    explicit Position(Node* node) : v(node) {}
  public:
    Position() : v(nullptr) {}
    
    string& element() const {
      if (!v) throw runtime_error("Posición nula");
      return v->element;
    }
    
    bool isNull() const { return v == nullptr; }
    
    bool operator==(const Position& o) const { return v == o.v; }
    bool operator!=(const Position& o) const { return v != o.v; }
  };
  
private:
  Node* rootNode  = nullptr;
  int   treeSize  = 0;
  
  void preOrderAux(Node* node, vector<string>& res) const {
    if (!node) return;
    res.push_back(node->element);
    for (Node* child : node->children)
      preOrderAux(child, res);
  }
  
  int deleteSubtree(Node* node) {
    if (!node) return 0;
    int count = 1;
    for (Node* child : node->children)
      count += deleteSubtree(child);
    delete node;
    return count;
  }
  
public:
  Tree() {}
  
  ~Tree() { deleteSubtree(rootNode); }
  
  bool isEmpty() const { return treeSize == 0; }
  int  size()    const { return treeSize; }
  
  Position root() const {
    if (!rootNode) throw runtime_error("Árbol vacío");
    return Position(rootNode);
  }
  
  Position addRoot(const string& value) {
    if (rootNode)
      throw runtime_error("El árbol ya tiene una raíz");
    rootNode = new Node(value);
    treeSize = 1;
    return Position(rootNode);
  }
  
  Position insert(const Position& parentPos, const string& value) {
    if (parentPos.isNull())
      return Position();                
    Node* parentNode = parentPos.v;
    
    Node* newNode = new Node(value, parentNode);
    parentNode->children.push_back(newNode);
    treeSize++;
    return Position(newNode);
  }
  
  Position parent(const Position& pos) const {
    if (pos.isNull() || !pos.v->parent)
      throw runtime_error("El nodo no existe o no tiene padre");
    return Position(pos.v->parent);
  }
  
  vector<Position> children(const Position& pos) const {
    vector<Position> result;
    if (pos.isNull()) return result;
    for (Node* child : pos.v->children)
      result.push_back(Position(child));
    return result;
  }
  
  bool remove(const Position& pos) {
    if (pos.isNull()) return false;
    Node* node = pos.v;
    
    if (node == rootNode) {
      deleteSubtree(rootNode);
      rootNode  = nullptr;
      treeSize  = 0;
      return true;
    }
    
    auto& siblings = node->parent->children;
    siblings.erase(
		   remove_if(siblings.begin(), siblings.end(),
			     [node](Node* c){ return c == node; }),
		   siblings.end()
		   );
    
    treeSize -= deleteSubtree(node);
    return true;
  }
  
  Position find(const string& value) const {
    return Position(searchNode(rootNode, value));
  }
  
  vector<string> preOrder() const {
    vector<string> result;
    preOrderAux(rootNode, result);
    return result;
  }
  
  vector<string> listar() { return preOrder(); }
  
private:
  Node* searchNode(Node* node, const string& value) const {
    if (!node) return nullptr;
    if (node->element == value) return node;
    for (Node* child : node->children) {
      Node* found = searchNode(child, value);
      if (found) return found;
    }
    return nullptr;
  }
};

#endif

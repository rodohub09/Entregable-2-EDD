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
  
  Node* findChild(Node* parent, const string& value) const {
    if (!parent) return nullptr;
    for (Node* child : parent->children)
      if (child->element == value)
        return child;
    return nullptr;
  }

  static int toInt(const string& text) {
    try {
      return stoi(text);
    } catch (...) {
      return 0;
    }
  }

  static double toDouble(const string& text) {
    try {
      return stod(text);
    } catch (...) {
      return 0.0;
    }
  }
  
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

  vector<string> listar() const {
    vector<string> result;
    if (!rootNode) return result;

    for (Node* libro : rootNode->children) {
      Node* idNode = findChild(libro, "ID");
      if (idNode && !idNode->children.empty())
        result.push_back(idNode->children.front()->element);
    }

    return result;
  }

  bool borrar_ratings(double r) {
    if (!rootNode) return false;

    vector<Position> eliminables;
    for (Node* libro : rootNode->children) {
      Node* ratingNode = findChild(libro, "Rating_Promedio");
      double rating = ratingNode && !ratingNode->children.empty()
        ? toDouble(ratingNode->children.front()->element)
        : 0.0;
      if (rating <= r)
        eliminables.push_back(Position(libro));
    }

    bool removedAny = false;
    for (const Position& pos : eliminables)
      removedAny = remove(pos) || removedAny;

    return removedAny;
  }

  vector<string> precursores() const {
    vector<string> result;
    if (!rootNode) return result;

    for (Node* libro : rootNode->children) {
      Node* yearNode = findChild(libro, "year");
      int year = yearNode && !yearNode->children.empty()
        ? toInt(yearNode->children.front()->element)
        : 0;

      bool qualifies = true;
      Node* similares = findChild(libro, "LibrosSimilares");
      if (similares) {
        for (Node* libroSimilar : similares->children) {
          Node* yearSimilar = findChild(libroSimilar, "year");
          int similarYear = yearSimilar && !yearSimilar->children.empty()
            ? toInt(yearSimilar->children.front()->element)
            : 0;

          if (similarYear <= year) {
            qualifies = false;
            break;
          }
        }
      }

      if (qualifies) {
        Node* idNode = findChild(libro, "ID");
        if (idNode && !idNode->children.empty())
          result.push_back(idNode->children.front()->element);
      }
    }

    return result;
  }
  
  Position find(const string& value) const {
    return Position(searchNode(rootNode, value));
  }
  
  vector<string> preOrder() const {
    vector<string> result;
    preOrderAux(rootNode, result);
    return result;
  }
  
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

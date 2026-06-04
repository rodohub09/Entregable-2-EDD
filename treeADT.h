#ifndef TREE_H
#define TREE_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

struct Node {
  string    element;
  Node* parent;
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
  
  // Busca secuencialmente un nodo hijo directo que coincida con el identificador ("ID", "year", etc.)
  Node* findChild(Node* parent, const string& value) const {
    if (!parent) return nullptr;
    for (Node* child : parent->children)
      if (child->element == value)
        return child;
    return nullptr;
  }

  // Convierte texto a entero
  static int toInt(const string& text) {
    try {
      return stoi(text);
    } catch (...) {
      return 0;
    }
  }

  // Convierte texto a punto flotante
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
  
  // Elimina de forma recursiva un nodo y todos sus descendientes para liberar memoria
  int deleteSubtree(Node* node) {
    if (!node) return 0;
    int count = 1;
    for (Node* child : node->children)
      count += deleteSubtree(child);
    delete node;
    return count;
  }

  // Función auxiliar para imprimir los vectores de strings resultantes
  static void imprimirLista (const string& titulo, const vector<string>& lista) {
    cout << "-> " << titulo << " (Total listado: " << lista.size() << "):\n   [ ";
    for (size_t i = 0; i < lista.size(); ++i) {
      cout << lista[i] << (i < lista.size() - 1 ? ", " : "");
    }
    cout << " ]\n\n";
  };
  
public:
  Tree() {}
  
  ~Tree() { deleteSubtree(rootNode); }
  
  bool isEmpty() const { return treeSize == 0; }
  int  size()    const { return treeSize; }

  // Retorna la raíz del árbol
  Position root() const {
    if (!rootNode) throw runtime_error("Árbol vacío");
    return Position(rootNode);
  }

  // Setter de la raíz del árbol
  Position addRoot(const string& value) {
    if (rootNode)
      throw runtime_error("El árbol ya tiene una raíz");
    rootNode = new Node(value);
    treeSize = 1;
    return Position(rootNode);
  }

  // Función que ingresa nuevos nodos al árbol
  Position insert(const Position& parentPos, const string& value) {
    if (parentPos.isNull())
      return Position();
    // Saca el nodo de la posición ingresada
    Node* parentNode = parentPos.v;

    // Crea el nuevo nodo con el valor ingresado y su padre 
    Node* newNode = new Node(value, parentNode);
    parentNode->children.push_back(newNode);
    treeSize++;
    return Position(newNode);
  }

  // Retorna el padre de un nodo
  Position parent(const Position& pos) const {
    if (pos.isNull() || !pos.v->parent)
      throw runtime_error("El nodo no existe o no tiene padre");
    return Position(pos.v->parent);
  }

  // Retorna todos los hijos de un nodo
  vector<Position> children(const Position& pos) const {
    vector<Position> result;
    if (pos.isNull()) return result;
    for (Node* child : pos.v->children)
      result.push_back(Position(child));
    return result;
  }
  
  // Desenlaza un nodo de su padre y destruye recursivamente toda su estructura subyacente
  bool remove(const Position& pos) {
    if (pos.isNull()) return false;
    Node* node = pos.v;
    
    if (node == rootNode) {
      deleteSubtree(rootNode);
      rootNode  = nullptr;
      treeSize  = 0;
      return true;
    }
    
    // Remueve el puntero del nodo desde la lista de hijos del nodo padre
    auto& siblings = node->parent->children;
    siblings.erase(
		   remove_if(siblings.begin(), siblings.end(),
			     [node](Node* c){ return c == node; }),
		   siblings.end()
		   );
    
    // Descuenta del tamaño total los nodos que pertenecían al subárbol eliminado
    treeSize -= deleteSubtree(node);
    return true;
  }

  // Recorre todos los libros del árbol y almacena los valores almanacedas en el hijo del nodo ID
  void listar() const {
    vector<string> result;
    if (!rootNode) return;

    for (Node* libro : rootNode->children) {
      Node* idNode = findChild(libro, "ID");
      if (idNode && !idNode->children.empty())
        result.push_back(idNode->children.front()->element);
    }

    imprimirLista("Resultado función listar()", result );
  
  }

  // Filtra los libros que tengan un rating menor o igual al valor r dado y los elimina del árbol
  void borrar_ratings(double r) {
    if (!rootNode) return;

    vector<Position> eliminables;
    for (Node* libro : rootNode->children) {
      Node* ratingNode = findChild(libro, "Rating_Promedio");
      double rating = ratingNode && !ratingNode->children.empty()
        ? toDouble(ratingNode->children.front()->element)
        : 0.0;
      
      // Si el libro califica para borrado, guardamos su posición
      if (rating <= r)
        eliminables.push_back(Position(libro));
    }


    // Elimina todos los libros que cumplen la condición
    for (const Position& pos : eliminables)
      remove(pos);

  }

  // Busca los libros que fueron publicados estrictamente antes que TODOS sus libros similares
  void precursores() const {
    vector<string> result;
    if (!rootNode) return;

    for (Node* libro_id : rootNode->children) {
      
      Node* idNode = findChild(libro_id, "ID");
      if (!idNode || idNode->children.empty()) continue;
      string id = idNode->children.front()->element;

      string year = "";
      Node* yearNode = findChild(libro_id, "year");
      if (yearNode && !yearNode->children.empty()) {
        year = yearNode->children.front()->element;
      }

      bool esPrecursorFlag = true;
      Node* similaresNode = findChild(libro_id, "LibrosSimilares");

      // Si el libro base no registra similares, no cumple el criterio de comparación estricta
      if (!similaresNode || similaresNode->children.empty()) {
        continue;
      }

      // Evaluamos la cronología de cada libro similar indexado
      for (Node* book : similaresNode->children) {
        Node* atributeNode = findChild(book, "year");
        
        if (!atributeNode || atributeNode->children.empty()) {
          esPrecursorFlag = false;
          break;
        }

        string similarYear = atributeNode->children.front()->element;

        // Si faltan datos cronológicos crípticos ("" o "0") en cualquiera de las partes, se invalida
        if (similarYear == "" || year == "" || similarYear == "0" || year == "0") {
          esPrecursorFlag = false;
          break;
        }

        // Si el libro base se publicó el mismo año o después que el similar, deja de ser precursor
        if (toInt(year) >= toInt(similarYear)) {
          esPrecursorFlag = false;
          break;
        }
      }

      if (esPrecursorFlag) {
        result.push_back(id);
      }
    }   

    imprimirLista("Resultado función precursores()", result);
    
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
  // Búsqueda en profundidad para localizar un nodo que posea el string exacto especificado
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

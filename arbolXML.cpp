#include "treeADT.h"
#include "pugixml.hpp"
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

int main(){

  // Creacion del arbol Biblioteca que almacenará todos los libros
  
  Tree Biblioteca;
  Tree::Position root = Biblioteca.addRoot("Biblioteca");

  // Ruta de la carpeta
  
  string carpeta = "books_xml"; 
  
  int archivosProcesados = 0;

  // Ciclo que itera sobre todos los archivos de la carpeta

  for (const auto& entry : fs::directory_iterator(carpeta)) {

    // Se verifica que cada archivo sea XML antes de avanzar
    
    if (entry.is_regular_file() && entry.path().extension() == ".xml") {

      // Carga del XML
      
      pugi::xml_document doc;
      pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
      
      if (result) {
	// Se selecciona el nodo dentro del XML que contiene al libro
	pugi::xml_node actual_book = doc.select_node("//GoodreadsResponse/book").node();
	// Se crea un nodo con el número del archivo procesado
	Tree::Position libro = Biblioteca.insert(root, "Libro_" + to_string(archivosProcesados));

	/** Las siguientes iteraciones crean un nodo para cada campo solicitado.
	    Primero se crea un nodo cuyo elemento es el campo a ingresar.
	    Luego se crea un nodo hijo con el valor asociado.
	 **/
        
	Tree::Position ID = Biblioteca.insert(libro, "ID");
        Biblioteca.insert(ID, string(actual_book.child_value("id")));
	
	Tree::Position titulo = Biblioteca.insert(libro, "Titulo");
        Biblioteca.insert(titulo, string(actual_book.child_value("title")));
	
	Tree::Position ISBN = Biblioteca.insert(libro, "ISBN");
        Biblioteca.insert(ISBN, string(actual_book.child_value("isbn")));
	
	Tree::Position year = Biblioteca.insert(libro, "year");
	Biblioteca.insert(year, string(actual_book.child_value("publication_year")));

	Tree::Position idioma = Biblioteca.insert(libro, "Idioma");
        Biblioteca.insert(idioma, string(actual_book.child_value("language_code")));
	
	Tree::Position descripcion = Biblioteca.insert(libro, "Descripcion");
        Biblioteca.insert(descripcion, string(actual_book.child_value("description")));
	
	Tree::Position rating = Biblioteca.insert(libro, "Rating_Promedio");
        Biblioteca.insert(rating, string(actual_book.child_value("average_rating")));
	
	Tree::Position paginas = Biblioteca.insert(libro, "Numero_paginas");
        Biblioteca.insert(paginas, string(actual_book.child_value("num_pages")));
	
	Tree::Position similares = Biblioteca.insert(libro, "LibrosSimilares");
	pugi::xpath_node_set libroSimilares = doc.select_nodes("//similar_books/book");

	// Para los libros similares se itera sobre todos los libros similares encontrados en el archivo XML del libro
	
	  for (pugi::xpath_node nodo : libroSimilares) {
	    pugi::xml_node libro = nodo.node();
	    Tree::Position libroSim = Biblioteca.insert(similares, "LibroSimilar");

	    // Los datos se almacenan de la misma forma en la que se guardan los campos del propio libro
            
	    Tree::Position ISBNsim = Biblioteca.insert(libroSim, "ISBN");
	    Biblioteca.insert(ISBNsim, string(libro.child_value("isbn")));
	    
	    Tree::Position yearSim = Biblioteca.insert(libroSim, "year");
	    Biblioteca.insert(yearSim, string(libro.child_value("publication_year")));
	    
	    Tree::Position tituloSim = Biblioteca.insert(libroSim, "titulo");
	    Biblioteca.insert(tituloSim, string(libro.child_value("title")));
	  }
	
	archivosProcesados++;
      }
    }
  }

 
  return 0;
}

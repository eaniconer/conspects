#pragma once
#ifndef LIST_H
#define LIST_H

#define DEFAULT_LIST_CAPACITY 0L

namespace basic {

	template <class Item>
	struct Node;

	template <class Item>
	class List {
	public:
		List() : _size(0L), _top(nullptr) {}
		List(List&);
		~List();
		List& operator=(const List& );
		
		long Count() const;
		bool Includes(const Item&) const;

		// Доступ к элементам
		Item& Get(long index) const;
		Item& First() const;
		Item& Last() const;		

		// Добавление
		void Append(const Item&);
		void Prepend(const Item&);

		// Удаление
		void Remove(const Item&);
		void RemoveLast();
		void RemoveFirst();
		void RemoveAll();

		// Интерфейс стека
		Item& Top() const;
		void Push(const Item&);
		Item& Pop();
	private:
		long _size;
		Node<Item>* _top;
	};

	template <class Item>
	struct Node{
		Item value;
		Node* prev;
		Node* next;

		Node() : prev(nullptr), next(nullptr) {}
		Node(const Item& val) : prev(nullptr), next(nullptr), value(val) {}
		~Node() 
		{ 
			if (prev != nullptr) delete prev;
			if (next != nullptr) delete next;
		}
	};
}
#include "list.tpp"
#endif // LIST_H
#include <stdexcept>

namespace basic 
{
	template <class Item>
	List<Item>::List(List& list) {
		Node<Item>* node = list._top;

		_size = 0;
		_top = nullptr;
		if (node == nullptr) return;

		Node<Item>* last = new Node<Item>(node->value);
		_top = last;
		_size = 1L;
		node = node->next;		
		while(node != nullptr) {
			last->next = new Node<Item>(node->value);
			last->next->prev = last;
			last = last->next;
			node = node->next;
			++_size;
		}
	}

	template <class Item>
	List<Item>::~List() {
		RemoveAll();
		_top = nullptr;
		delete _top;
	}

	template <class Item>
	long basic::List<Item>::Count() const
	{
		return _size;
	}

	template <class Item>
	bool List<Item>::Includes(const Item& item) const 
	{
		Node<Item>* node = _top;
		while(node != nullptr) {
			if (node->value == item) return true;
			node = node->next;
		}
		return false;
	}

	template <class Item>
	Item& List<Item>::Get(long index) const 
	{
		Node<Item>* node = _top;
		while(index > 0 && node != nullptr) {
			--index;
			node = node->next;
		}
		if (node == nullptr){
			throw std::out_of_range::out_of_range;
		}

		return node->value;
	}

	template <class Item>
	Item& List<Item>::First() const
	{
		return Get(0);
	}

	template <class Item>
	Item& List<Item>::Last() const
	{
		return Get(Count() - 1);
	}

	template <class Item>
	void List<Item>::Append(const Item& item) {
		
		if (_top == nullptr) {
			_top = new Node<Item>(item);
		} else {
			Node<Item>* node = _top;
			while (node->next != nullptr) 
			{
				node = node->next;
			}
			node->next = new Node<Item>(item);
			node->next->prev = node;
		}
		++_size;
	}

	template <class Item>
	void List<Item>::Prepend(const Item& item) {
		Node<Item>* node = new Node<Item>(item);
		node->next = _top;
		if (_top != nullptr) {
			_top->prev = node;
		}		
		_top = node;

		++_size;
	}

	// Удаление
	template <class Item>
	void List<Item>::Remove(const Item& item) {
		Node<Item>* node = _top;
		while(node != nullptr) 
		{
			if (node->value == item)
			{
				Node<Item>* tmp = node->prev;
				if (tmp != nullptr) 
				{
					tmp->next = node->next;
				} else {
					_top = node->next;
				}
				tmp = node->next;
				if (tmp != nullptr) 
				{
					tmp->prev = node->prev;
				}
				--_size;
				
				node->next = nullptr;
				node->prev = nullptr;
				delete node;

				return;
			} 
			node = node->next;
		}
	}

	template <class Item>
	void List<Item>::RemoveFirst() {
		Remove(First());
	}

	template <class Item>
	void List<Item>::RemoveLast() 
	{
		Remove(Last()); //todo - optimize
	}

	template <class Item>
	void List<Item>::RemoveAll() 
	{
		while(_size > 0) {
			RemoveFirst();
		}
	}

	// Интерфейс стека (для удобства)

	template <class Item>
	Item& List<Item>::Top() const
	{
		return First();
	}

	template <class Item>
	void List<Item>::Push(const Item& item) 
	{
		Prepend(item);
	}

	template <class Item>
	Item& List<Item>::Pop() 
	{
		Item& top = First();
		RemoveFirst();
		return top;
	}
}



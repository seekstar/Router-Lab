#ifndef MYLIST_H_
#define MYLIST_H_

template <typename T>
struct MyList_node {
	T v;
	MyList_node<T> *prv, *nxt;
};

template <typename T>
struct MyList_iterator {
//private:
	typedef MyList_node<T> node;

	node* cur;

//public:
	typedef MyList_iterator<T> iterator;

	MyList_iterator() = default;
	MyList_iterator(node* now) : cur(now) {}

	//prefix
	iterator& operator ++ () {
		cur = cur->nxt;
		return *this;
	}
	//prefix
	iterator& operator -- () {
		cur = cur->prv;
		return *this;
	}
	//prefix
	T operator * () {
		return cur->v;
	}

	bool operator != (const iterator& b) const {
		return cur != b.cur;
	}
};

template <typename T>
struct MyList {
private:
	typedef MyList_node<T> node;

	node hd;	//head

public:
	typedef MyList_iterator<T> iterator;

	MyList() {
		hd.prv = hd.nxt = &hd;
	}

	void push_back(T x) {
		hd.prv = hd.prv->nxt = new node{x, hd.prv, &hd};
	}
	iterator begin() {
		return iterator(hd.nxt);
	}
	iterator end() {
		return iterator(&hd);
	}

    T front() const {
        return hd.nxt->v;
    }
    void pop_front() {
        hd.nxt = hd.nxt->nxt;
        delete hd.nxt->prv;
        hd.nxt->prv = &hd;
    }
	void pick_out(iterator it) {
		node* now = it.cur;
        now->prv->nxt = now->nxt;
        now->nxt->prv = now->prv;
	}
    void erase(iterator it) {
        pick_out(it);
        delete it.cur;
    }
	//insert it in front of pos
	void insert(iterator pos, iterator it) {
		pos.cur->prv->nxt = it.cur;
		it.cur->prv = pos.cur->prv;
		it.cur->nxt = pos.cur;
		pos.cur->prv = it.cur;
	}
};

#endif	//MYLIST_H_

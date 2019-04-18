#ifndef TREE_H
#define TREE_H

#include <QList>
#include <QtGlobal>
#include <stack>
#include <type_traits>

/**
 * @brief General purpose tree mixin
 * Use it by CRTP pattern as
 * class MyNode: public MixinTreeNode<MyNode>
 */
template<typename T>
class MixinTreeNode
{
    // alias
    typedef MixinTreeNode<T> Node;

protected:
    // You must inherit from this class
    MixinTreeNode()
        : mParent(nullptr)
    {
        // This can catch some errors, but not all
        static_assert(std::is_base_of<MixinTreeNode<T>, T>::value,
                      "You must derive T from MixinTreeNode<T>");
    }

public:
    virtual ~MixinTreeNode();
    // FIXME: rule of three!

    // Mixin (CRTP) magic
    // We require that MixinTreeNode<T> is base of T!
    // Abuse leads to undefined behavoir

    T* self() { return static_cast<T*>(this); }
    const T* self() const { return static_cast<const T*>(this); }
    // Tree-like logic
    inline bool isChild() const { return mParent != nullptr; }
    inline Node* parent() const { return mParent; }
    // find my index among sibling nodes
    int myIndex() const;
    // find index of child node
    int indexOf(const T* child) const;

    T* child(int i) const { return mChilds[i]; }
    int childCount() const { return mChilds.size(); }

    // takes ownership
    void appendChild(T* child) { insertChild(mChilds.size(), child); }
    virtual bool insertChild(int position, T* child);
    virtual bool insertChildren(int position, QVector<T*> list);
    // deletes underline data
    bool removeChildren(int position, int count);
    void clear() { removeChildren(0, mChilds.size()); }
    // releases ownership
    virtual QVector<T*> takeChildren(int position, int count);

    /**
     * @brief DFS tree iterator
     */
    class Iterator
    {
        friend class MixinTreeNode<T>;

    public:
        bool operator!=(const Iterator& other)
        {
            return pNode != other.pNode || pStack != other.pStack;
        }

        Iterator& operator++();

        // you need to compare with dfs_end() before calling this
        T& operator*() { return *(node()->self()); }
        T* operator->() { return node()->self(); }

    private:
        enum BeginEnd
        {
            Begin,
            End
        };
        Iterator(Node* root, BeginEnd type);
        inline Node* node();

        // references
        Node* pNode;
        std::stack<Node*> pStack;
    };

    Iterator dfs_begin() { return Iterator(this, Iterator::Begin); }
    Iterator dfs_end() { return Iterator(this, Iterator::End); }

private:
    // ref
    Node* mParent;
    // own
    QList<T*> mChilds;
};

// Implementation
// MixinTreeNode

template<typename T>
MixinTreeNode<T>::~MixinTreeNode()
{
    qDeleteAll(mChilds);
    mChilds.clear();
}

template<typename T>
int MixinTreeNode<T>::myIndex() const
{
    // FIXME: not that smart way to find index
    if (mParent) {
        return mParent->indexOf(self());
    }
    return -1;
}

template<typename T>
int MixinTreeNode<T>::indexOf(const T* child) const
{
    // I believe that indexOf() doesn't modify the object
    return mChilds.indexOf(const_cast<T*>(child));
}

template<typename T>
bool MixinTreeNode<T>::insertChild(int position, T* child)
{
    if (position < 0 || position > mChilds.count()) {
        return false;
    }
    child->mParent = this;
    mChilds.insert(position, child);
    return true;
}

template<typename T>
bool MixinTreeNode<T>::insertChildren(int position, QVector<T*> list)
{
    if (position < 0 || position > mChilds.count()) {
        return false;
    }
    auto it = mChilds.begin() + position;
    for (int i = 0; i < list.count(); i++) {
        list[i]->mParent = this;
        it = mChilds.insert(it, list[i]); // Inserts before iterator
        it++;
    }
    return true;
}

template<typename T>
bool MixinTreeNode<T>::removeChildren(int position, int count)
{
    if (position < 0 || position + count > mChilds.size()) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        delete mChilds.takeAt(position);
    }
    return true;
}

template<typename T>
QVector<T*> MixinTreeNode<T>::takeChildren(int position, int count)
{
    if (position < 0 || position + count > mChilds.size()) {
        return QVector<T*>();
    }
    QVector<T*> list;
    for (int row = 0; row < count; ++row) {
        auto child = mChilds.takeAt(position);
        child->mParent = nullptr;
        list.append(child);
    }
    return list;
}

// Implementation
// MixinTreeNode::Iterator

template<typename T>
MixinTreeNode<T>::Iterator::Iterator(Node* root, BeginEnd type)
    : pNode(root)
{
    switch (type) {
    case Begin:
        pStack.push(pNode);
        break;
    case End:
        break;
    }
}

template<typename T>
typename MixinTreeNode<T>::Iterator& MixinTreeNode<T>::Iterator::operator++()
{
    if (pStack.empty())
        return *this;

    Node* v = pStack.top();
    pStack.pop();
    for (Node* w : v->mChilds) {
        pStack.push(w);
    }
    return *this;
}

template<typename T>
typename MixinTreeNode<T>::Node* MixinTreeNode<T>::Iterator::node()
{
    if (!pStack.empty()) {
        return pStack.top();
    } else {
        return nullptr;
    }
}

#endif // TREE_H

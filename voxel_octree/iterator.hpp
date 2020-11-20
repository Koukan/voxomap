#ifndef _VOXOMAP_ITERATOR_HPP_
#define _VOXOMAP_ITERATOR_HPP_

#include <cstdint>

namespace voxomap
{

template <typename T_SuperContainer> class VoxelNode;

/*!
	\addtogroup Iterator Iterator
	\ingroup VoxelOctree
	Classes used to implement iterator of VoxelOctree
*/
/*!
	\ingroup Iterator
*/
template <typename T>
struct container_iterator
{
	using VoxelData = typename T::VoxelData;
	using VoxelContainer = typename T::VoxelContainer;

	VoxelNode<T>* node = nullptr;				//!< Pointer on node
	VoxelContainer* voxelContainer = nullptr;	//!< Pointer on voxel container
	VoxelData* voxel = nullptr;					//!< Pointer on voxel
	uint8_t x = 0;								//!< x coordinate of \a voxel inside \a node
	uint8_t y = 0;								//!< y coordinate of \a voxel inside \a node
	uint8_t z = 0;								//!< z coordinate of \a voxel inside \a node

	/*!
		\brief Pre-increment the iterator
		\return Reference on \a this
	*/
	container_iterator& operator++();
	/*!
		\brief Post increment the iterator
		\return Not incremented iterator
	*/
	container_iterator operator++(int);
	/*!
		\brief Dereference iterator
	*/
	container_iterator* operator*();
	/*!
		\brief Returns true if the iterator is valid
	*/
	operator bool() const;
	/*!
		\brief Returns true if the iterator is same as \a other
	*/
	bool operator==(container_iterator const& other) const;
	/*!
		\brief Returns true if the iterator is different as \a other
	*/
	bool operator!=(container_iterator const& other) const;

    /*!
        \brief Initialize \a this with the first voxel of \a node
		\param node
    */
    void begin(VoxelNode<T>& node);
    /*!
        \brief Initialize \a this with the next element after the last voxel of \a node
		\param node
	*/
    void end(VoxelNode<T>& node);

	/*!
		\brief Get the absolute position of the voxel
		\param x X position of voxel
		\param y Y position of voxel
		\param z Z position of voxel
	*/
	void getVoxelPosition(int& x, int& y, int& z) const;
	/*!
		\brief Get the relative position of the voxel inside its node
		\param x X position of voxel
		\param y Y position of voxel
		\param z Z position of voxel
	*/
	void getRelativeVoxelPosition(int& x, int& y, int& z) const;
	/*!
		\brief Initiliaze the iterator with the absolute position
		\param x X position of the voxel
		\param y Y position of the voxel
		\param z Z position of the voxel
	*/
	void initPosition(int x, int y, int z);

	int getRelativeX() const;
	int getRelativeY() const;
	int getRelativeZ() const;
	int getAbsoluteX() const;
	int getAbsoluteY() const;
	int getAbsoluteZ() const;

	container_iterator<T> findRelativeVoxel(int x, int y, int z);

protected:
	void findNextParentNode(VoxelNode<T>& i_node);
	bool findNextChildNode(VoxelNode<T>& i_node);
	bool findNextVoxel(VoxelContainer& container);
};

/*!
* \ingroup Iterator
*/
template <typename T>
struct supercontainer_iterator : container_iterator<T>
{
	using Container = typename T::Container;
	using VoxelContainer = typename container_iterator<T>::VoxelContainer;
	struct Position
	{
		uint8_t x = 0;
		uint8_t y = 0;
		uint8_t z = 0;
	};
	
	Position containerPosition[T::NB_SUPERCONTAINER]; //!< Internal positions of the containers

	/*!
		\brief Pre-increment the iterator
		\return Reference on \a this
	*/
	supercontainer_iterator& operator++();
	/*!
		\brief Post increment the iterator
		\return Not incremented iterator
	*/
	supercontainer_iterator operator++(int);
	/*!
		\brief Dereference iterator
	*/
    supercontainer_iterator* operator*();
	/*!
		\brief Returns true if the iterator is same as \a other
	*/
	bool operator==(supercontainer_iterator const& other) const;
	/*!
		\brief Returns true if the iterator is different as \a other
	*/
	bool operator!=(supercontainer_iterator const& other) const;

	/*!
		\brief Initialize \a this with the first voxel of \a node
		\param node
	*/
	void begin(VoxelNode<T>& node);
	/*!
		\brief Initialize \a this with the next element after the last voxel of \a node
		\param node
	*/
	void end(VoxelNode<T>& node);

	/*!
		\brief Get the absolute position of the voxel
		\param x X position of voxel
		\param y Y position of voxel
		\param z Z position of voxel
	*/
    void getVoxelPosition(int& x, int& y, int& z) const;
	/*!
		\brief Get the relative position of the voxel inside its node
		\param x X position of voxel
		\param y Y position of voxel
		\param z Z position of voxel
	*/
	void getRelativeVoxelPosition(int& x, int& y, int& z) const;
	/*!
		\brief Initiliaze the iterator with the absolute position
		\param x X position of the voxel
		\param y Y position of the voxel
		\param z Z position of the voxel
	*/
	void initPosition(int x, int y, int z);

	int getRelativeX() const;
	int getRelativeY() const;
	int getRelativeZ() const;
	int getAbsoluteX() const;
	int getAbsoluteY() const;
	int getAbsoluteZ() const;

	supercontainer_iterator<T> findRelativeVoxel(int x, int y, int z);

private:
	bool findNextChildNode(VoxelNode<T>& node);
	void findNextParentNode(VoxelNode<T>& node);

	template <class T_Container>
	typename std::enable_if<(T_Container::NB_SUPERCONTAINER != 0 && T_Container::SUPERCONTAINER_ID != 0), bool>::type findNextContainer(T_Container& container);

	template <class T_Container>
	typename std::enable_if<(T_Container::NB_SUPERCONTAINER != 0 && T_Container::SUPERCONTAINER_ID == 0), bool>::type findNextContainer(T_Container& container);

	template <class T_Container>
	typename std::enable_if<(T_Container::NB_SUPERCONTAINER == 0), bool>::type findNextContainer(T_Container& container);
};

}

#include "iterator.ipp"

#endif // _VOXOMAP_ITERATOR_HPP_

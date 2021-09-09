#include <iostream>
#include "QuadTree.h"

QuadTreeNodeData QuadTreeNodeDataMake(double latitude, double longitude, void *data) {
    
    QuadTreeNodeData nodeData;
    nodeData.latitude = latitude;
    nodeData.longitude = longitude;
    nodeData.data = data;
    
    return nodeData;
    
}

BoundingBox BoundingBoxMake(double x0, double y0, double x1, double y1) {
    
    BoundingBox boundingBox;
    boundingBox.x0 = x0;
    boundingBox.y0 = y0;
    boundingBox.x1 = x1;
    boundingBox.y1 = y1;
    
    return boundingBox;
    
}

bool BoundingBoxContainsData(BoundingBox boundary, QuadTreeNodeData data) {
    
    bool containsX = boundary.x0 <= data.latitude && data.latitude <= boundary.x1;
    bool containsY = boundary.y0 <= data.longitude && data.longitude <= boundary.y1;
    
    return containsX && containsY;
    
}

bool BoundingBoxIntersectsBoundingBox(BoundingBox boundary, BoundingBox test) {
    
    return (boundary.x0 <= test.x1 && boundary.x1 >= test.x0 && boundary.y0 <= test.y1 && boundary.y1 >= test.y0);
    
}

QuadTreeNode::QuadTreeNode(BoundingBox boundary, int capacity) {
    
    this->northWest = NULL;
    this->northEast = NULL;
    this->southWest = NULL;
    this->southEast = NULL;
    
    this->boundary = boundary;
    this->capacity = capacity;
    this->count = 0;
    this->points = (QuadTreeNodeData *)malloc ( sizeof ( QuadTreeNodeData ) * capacity );
    
}

QuadTreeNode::~QuadTreeNode() {
    
    if ( this->northWest != NULL ) delete this->northWest;
    if ( this->northEast != NULL ) delete this->northEast;
    if ( this->southWest != NULL ) delete this->southWest;
    if ( this->southEast != NULL ) delete this->southEast;
    
    for ( int i = 0; i < this->count; i++ ) {
        free ( this->points[i].data );
    }
    
    delete this->points;
    
}

QuadTreeNode* QuadTreeNode::QuadTreeNodeMake(BoundingBox boundary, int capacity) {
    
    QuadTreeNode *node = new QuadTreeNode ( boundary, capacity );	
    return node;
    
}

void QuadTreeNode::gatherDataWithinBoundary(BoundingBox boundary, QuadTreeNodeDataBlock block) {
    
    if ( !BoundingBoxIntersectsBoundingBox ( this->boundary, boundary ) ) {
        return;
    }
    
    for ( int i = 0; i < this->count; i++ ) {
        if ( BoundingBoxContainsData ( boundary, this->points[i] ) ) {
            block ( this->points[i] );
        }
    }
    
    if ( this->northWest == NULL ) {
        return;
    }
    
    this->northWest->gatherDataWithinBoundary ( boundary, block );
    this->northEast->gatherDataWithinBoundary ( boundary, block );
    this->southWest->gatherDataWithinBoundary ( boundary, block );
    this->southEast->gatherDataWithinBoundary ( boundary, block );
    
}

void QuadTreeNode::traverse(QuadTreeNodeTraverseBlock block) {
    
    block ( this );
    
    if ( this->northWest == NULL ) {
        return;
    }
    
    this->northWest->traverse ( block );
    this->northEast->traverse ( block );
    this->southWest->traverse ( block );
    this->southEast->traverse ( block );
    
}

bool QuadTreeNode::insert(QuadTreeNodeData data) {
    
    if ( !BoundingBoxContainsData ( this->boundary, data ) ) {
        return false;
    }
    
    if ( this->count < this->capacity ) {
        this->points[this->count++] = data;
        return true;
    }
    
    if ( this->northWest == NULL ) {
        this->subdivide();
    }
    
    if ( this->northWest->insert ( data ) ) return true;
    if ( this->northEast->insert ( data ) ) return true;
    if ( this->southWest->insert ( data ) ) return true;
    if ( this->southEast->insert ( data ) ) return true;
    
    return false;
    
}

void QuadTreeNode::subdivide() {
    
    BoundingBox box = this->boundary;
    
    double xMid = (box.x1 + box.x0) / 2.0;
    double yMid = (box.y1 + box.y0) / 2.0;
    
    BoundingBox northWest = BoundingBoxMake ( box.x0, box.y0, xMid, yMid );
    this->northWest = new QuadTreeNode ( northWest, this->capacity );
    
    BoundingBox northEast = BoundingBoxMake ( xMid, box.y0, box.x1, yMid );
    this->northEast = new QuadTreeNode ( northEast, this->capacity );
    
    BoundingBox southWest = BoundingBoxMake ( box.x0, yMid, xMid, box.y1 );
    this->southWest = new QuadTreeNode ( southWest, this->capacity );
    
    BoundingBox southEast = BoundingBoxMake ( xMid, yMid, box.x1, box.y1 );
    this->southEast = new QuadTreeNode ( southEast, this->capacity );
    
}

QuadTreeNodeData* QuadTreeNode::getPoints() const {
    
    return this->points;
    
}

BoundingBox QuadTreeNode::getBoundary() const {
    
    return this->boundary;
    
}

int QuadTreeNode::getCapacity() const {
    
    return this->capacity;
    
}

QuadTree::QuadTree(QuadTreeNodeData *data, BoundingBox bourdary, int capacity) : QuadTreeNode ( boundary, capacity ) {
    
    for ( int i = 0; i < sizeof ( data ) / sizeof ( QuadTreeNodeData ); i++ ) {
        this->insert ( data[i] );
    }
    
}

QuadTreeNode* QuadTreeMake(QuadTreeNodeData *data, BoundingBox boundary, int capacity) {
    
    QuadTree *tree = new QuadTree ( data, boundary, capacity );
    return tree;
    
}


#ifndef Buildings_QuadTreeNode_h
#define Buildings_QuadTreeNode_h

class QuadTreeNodeData;
class QuadTreeNode;

typedef void(^QuadTreeNodeDataBlock)(QuadTreeNodeData data);
typedef void(^QuadTreeNodeTraverseBlock)(QuadTreeNode *node);

typedef struct QuadTreeNodeData {
    double latitude;
    double longitude;
    void *data;
} QuadTreeNodeData;

QuadTreeNodeData QuadTreeNodeDataMake(double latitude, double longitude, void *data);

typedef struct BoundingBox {
    double x0; double y0;
    double x1; double y1;
} BoundingBox;

BoundingBox BoundingBoxMake(double x0, double y0, double x1, double y1);

bool BoundingBoxContainsData(BoundingBox boundary, QuadTreeNodeData data);
bool BoundingBoxIntersectsBoundingBox(BoundingBox boundary, BoundingBox test);

class QuadTreeNode {
protected:
    QuadTreeNode *northWest;
    QuadTreeNode *northEast;
    QuadTreeNode *southWest;
    QuadTreeNode *southEast;
    QuadTreeNodeData *points;
    BoundingBox boundary;
    int capacity;
    int count;
public:
    QuadTreeNode(BoundingBox boundary, int capacity);
    ~QuadTreeNode();
    
    static QuadTreeNode* QuadTreeNodeMake(BoundingBox boundary, int capacity);
    
    void gatherDataWithinBoundary(BoundingBox boundary, QuadTreeNodeDataBlock block);
    void traverse(QuadTreeNodeTraverseBlock block);
    bool insert(QuadTreeNodeData data);
    void subdivide();
    
    QuadTreeNodeData* getPoints() const;
    BoundingBox getBoundary() const;
    int getCapacity() const;
};

class QuadTree : public QuadTreeNode {
public:
    QuadTree(QuadTreeNodeData *data, BoundingBox bourdary, int capacity);
    static QuadTree* QuadTreeMake(QuadTreeNodeData *data, BoundingBox boundary, int capacity);
};

#endif
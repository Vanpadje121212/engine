/****************************************************************************
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.
 
 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "scene/Node.h"
#include "scene/Define.h"

namespace cc {
namespace scene {
void Node::updateWorldTransform() {
    if (!getDirtyFlag()) {
        return;
    }
    uint32_t   i    = 0;
    Node *     curr = this;
    Mat3       mat3;
    Mat3       m43;
    Quaternion quat;
    while (curr && curr->getDirtyFlag()) {
        i++;
        _computeNodes.push_back(curr);
        curr = curr->getParent();
    }
    Node *   child{nullptr};
    uint32_t dirtyBits = 0;
    while (i) {
        child = _computeNodes[--i];
        if (!child) {
            continue;
        }
        dirtyBits |= child->getDirtyFlag();
        auto *childLayout = child->_nodeLayout;
        if (curr) {
            if (dirtyBits & static_cast<uint32_t>(TransformBit::POSITION)) {
                childLayout->worldPosition.transformMat4(childLayout->localPosition, childLayout->worldMatrix);
                childLayout->worldMatrix.m[12] = childLayout->worldPosition.x;
                childLayout->worldMatrix.m[13] = childLayout->worldPosition.y;
                childLayout->worldMatrix.m[14] = childLayout->worldPosition.z;
            }
            if (dirtyBits & static_cast<uint32_t>(TransformBit::RS)) {
                auto *currLayout = curr->_nodeLayout;
                Mat4::fromRTS(childLayout->localRotation, childLayout->localPosition, childLayout->localScale, &childLayout->worldMatrix);
                Mat4::multiply(currLayout->worldMatrix, childLayout->worldMatrix, &childLayout->worldMatrix);
                if (dirtyBits & static_cast<uint32_t>(TransformBit::ROTATION)) {
                    Quaternion::multiply(currLayout->worldRotation, childLayout->localRotation, &currLayout->worldRotation);
                }
                quat = childLayout->worldRotation;
                quat.conjugate();
                Mat3::fromQuat(mat3, quat);
                Mat3::fromMat4(m43, childLayout->worldMatrix);
                Mat3::multiply(mat3, mat3, m43);
                childLayout->worldScale.set(mat3.m[0], mat3.m[4], mat3.m[8]);
            }
        } else if (child) {
            if (dirtyBits & static_cast<uint32_t>(TransformBit::POSITION)) {
                childLayout->worldPosition.set(childLayout->localPosition);
                childLayout->worldMatrix.m[12] = childLayout->worldPosition.x;
                childLayout->worldMatrix.m[13] = childLayout->worldPosition.y;
                childLayout->worldMatrix.m[14] = childLayout->worldPosition.z;
            }
            if (dirtyBits & static_cast<uint32_t>(TransformBit::RS)) {
                if (dirtyBits & static_cast<uint32_t>(TransformBit::ROTATION)) {
                    childLayout->worldRotation.set(childLayout->localRotation);
                }
                if (dirtyBits & static_cast<uint32_t>(TransformBit::SCALE)) {
                    childLayout->worldScale.set(childLayout->localScale);
                    Mat4::fromRTS(childLayout->localRotation, childLayout->localPosition, childLayout->localScale, &childLayout->worldMatrix);
                }
            }
        }
        child->setDirtyFlag(static_cast<uint32_t>(TransformBit::NONE));
        curr = child;
    }
}

void Node::updateWorldRTMatrix() {
    updateWorldTransform();
    Mat4::fromRT(_nodeLayout->worldRotation, _nodeLayout->worldPosition, &_rtMat);
}

void Node::invalidateChildren(TransformBit dirtyBit) {
    uint32_t       curDirtyBit{static_cast<uint32_t>(dirtyBit)};
    const uint32_t childDirtyBit{curDirtyBit | static_cast<uint32_t>(TransformBit::POSITION)};
    if (static_cast<int>(_computeNodes.size()) == 0) {
        _computeNodes.resize(1);
    }
    _computeNodes[0] = this;
    int i{0};
    while (i >= 0) {
        Node *          cur             = _computeNodes[i--];
        const uint32_t &hasChangedFlags = cur->getFlagsChanged();
        if ((cur->getDirtyFlag() & hasChangedFlags & curDirtyBit) != curDirtyBit) {
            cur->setDirtyFlag(cur->getDirtyFlag() | curDirtyBit);
            cur->setFlagsChanged(hasChangedFlags | curDirtyBit);
            int childCount{static_cast<int>(cur->_children.size())};
            if (childCount > 0) {
                _computeNodes.resize(i < 0 ? childCount : childCount + i);
            }
            for (Node *curChild : cur->_children) {
                _computeNodes[++i] = curChild;
            }
        }
        curDirtyBit = childDirtyBit;
    }
}

void Node::setWorldPosition(float x, float y, float z) {
    _nodeLayout->worldPosition.set(x, y, z);
    if (_parent) {
        Mat4 invertWMat{_parent->getWorldMatrix()};
        _parent->updateWorldTransform();
        invertWMat.inverse();
        _nodeLayout->localPosition.transformMat4(_nodeLayout->worldPosition, invertWMat);
    } else {
        _nodeLayout->localPosition.set(_nodeLayout->worldPosition);
    }
    invalidateChildren(TransformBit::POSITION);
}

void Node::setWorldRotation(float x, float y, float z, float w) {
    _nodeLayout->worldRotation.set(x, y, z, w);
    if (_parent) {
        _parent->updateWorldTransform();
        _nodeLayout->localRotation.set(_parent->_nodeLayout->worldRotation.getConjugated());
        _nodeLayout->localRotation.multiply(_nodeLayout->worldRotation);
    } else {
        _nodeLayout->localRotation.set(_nodeLayout->worldRotation);
    }
    invalidateChildren(TransformBit::ROTATION);
}

void Node::setParent(Node *parent) {
    if (_parent == parent) {
        return;
    }
    if (_parent != nullptr) {
        _parent->removeChild(this);
    }
    _parent = parent;
    if (_parent) {
        _parent->addChild(this);
    }
}

void Node::initWithData(uint8_t *data, uint8_t *flagChunk, uint32_t offset) {
    _nodeLayout = reinterpret_cast<NodeLayout *>(data);
    _flagChunk  = flagChunk;
    _flagOffest = offset;
}

} // namespace scene
} // namespace cc

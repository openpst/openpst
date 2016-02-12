/**
* LICENSE PLACEHOLDER
*
* @file dm_efs_node.cpp
* @class OpenPST::DmEfsNode
* @package OpenPST
* @brief Represents a file or dir retrieved from the diagnostic monitor EFS subsystem
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "dm_efs_node.h"

using namespace OpenPST::QC;

/**
* @brief DmEfsNode - Constructor
* @param QcdmSerial port
*/
DmEfsNode::DmEfsNode(std::string path, QcdmEfsReadDirResponse* data) :
    path(path),
    name(data->name),
    error(data->error),
    type(data->entryType),
    mode(data->mode),
    size(data->size),
    atime(data->atime),
    mtime(data->mtime),
    ctime(data->ctime)
{
}

/**
* @brief ~DmEfsNode - Deconstructor
*/
DmEfsNode::~DmEfsNode()
{

}

/**
* @brief Copy Constructor
*/
DmEfsNode::DmEfsNode(const DmEfsNode& org) :
    path(org.path),
    name(org.name),
    error(org.error),
    type(org.type),
    mode(org.mode),
    size(org.size),
    atime(org.atime),
    mtime(org.mtime),
    ctime(org.ctime),
    children(org.children)
{

}

bool DmEfsNode::isDir()
{
    return type == kDiagEfsFileTypeDir;
}

bool DmEfsNode::isFile()
{
    return type == kDiagEfsFileTypeFile;
}

bool DmEfsNode::isLink()
{
    return type == kDiagEfsFileTypeLink;
}

bool DmEfsNode::isImmovable()
{
    return type == kDiagEfsFileTypeImmovable;
}

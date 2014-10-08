/*
 *  Copyright (C) 2011-2014  OpenDungeons Team
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TreasuryObject.h"

#include "GameMap.h"
#include "RoomTreasury.h"
#include "LogManager.h"

#include <iostream>

TreasuryObject::TreasuryObject(GameMap* gameMap, int goldValue) :
    RoomObject(gameMap, "Treasury_", "GoldstackLv3", 0.0f),
    mGoldValue(goldValue)
{
}

TreasuryObject::TreasuryObject(GameMap* gameMap) :
    RoomObject(gameMap),
    mGoldValue(0)
{
    setMeshName("GoldstackLv3");
}

void TreasuryObject::mergeGold(TreasuryObject* obj)
{
    mGoldValue += obj->mGoldValue;
    obj->mGoldValue = 0;
}

void TreasuryObject::addGold(int goldValue)
{
    mGoldValue += goldValue;
}

void TreasuryObject::doUpkeep()
{
    if(!getIsOnMap())
        return;

    // We check if we are on a tile where there is a treasury room. If so, we add gold there
    Tile* tile = getPositionTile();
    OD_ASSERT_TRUE_MSG(tile != nullptr, "tile=" + Tile::displayAsString(tile));
    if(tile == nullptr)
        return;

    if(mGoldValue > 0)
    {
        Room* room = tile->getCoveringRoom();
        if(room != nullptr)
        {
            if(room->getType() == Room::RoomType::treasury)
            {
                RoomTreasury* roomTreasury = static_cast<RoomTreasury*>(room);
                int goldDeposited = roomTreasury->depositGold(mGoldValue, tile);
                // We withdraw the amount we could deposit
                addGold(-goldDeposited);
            }
        }
    }

    // If we are empty, we can remove safely
    if(mGoldValue <= 0)
    {
        tile->removeTreasuryObject(this);
        getGameMap()->removeRoomObject(this);
        deleteYourself();
    }
}

bool TreasuryObject::tryPickup(Seat* seat, bool isEditorMode)
{
    if(!getIsOnMap())
        return false;

    // We do not let it be picked up as it will be removed during next upkeep. However, this is
    // true only on server side. On client side, if a treasury is available, it can be picked up.
    if(getGameMap()->isServerGameMap() && (mGoldValue <= 0))
        return false;

    Tile* tile = getPositionTile();
    OD_ASSERT_TRUE_MSG(tile != NULL, "tile=" + Tile::displayAsString(tile));
    if(tile == NULL)
        return false;

    if(!tile->isClaimedForSeat(seat) && !isEditorMode)
        return false;

    return true;
}

void TreasuryObject::pickup()
{
    Tile* tile = getPositionTile();
    RoomObject::pickup();
    OD_ASSERT_TRUE_MSG(tile != nullptr, "tile=" + Tile::displayAsString(tile));
    if(tile == nullptr)
        return;

    tile->removeTreasuryObject(this);
}

bool TreasuryObject::tryDrop(Seat* seat, Tile* tile, bool isEditorMode)
{
    if (tile->getFullness() > 0.0)
        return false;

    // In editor mode, we allow to drop an object in dirt, claimed or gold tiles
    if(isEditorMode && (tile->getType() == Tile::dirt || tile->getType() == Tile::gold || tile->getType() == Tile::claimed))
        return true;

    // Otherwise, we allow to drop an object only on allied claimed tiles
    if(tile->getType() == Tile::claimed && tile->getSeat() != NULL && tile->getSeat()->isAlliedSeat(seat))
        return true;

    return false;
}

void TreasuryObject::setPosition(const Ogre::Vector3& v)
{
    RoomObject::setPosition(v);
    Tile* tile = getPositionTile();
    OD_ASSERT_TRUE_MSG(tile != nullptr, "tile=" + Tile::displayAsString(tile));
    if(tile == nullptr)
        return;

    tile->addTreasuryObject(this);

}

const char* TreasuryObject::getFormat()
{
    // TODO : implement saving/loading in the level file
    return "position\tvalue";
}

TreasuryObject* TreasuryObject::getTreasuryObjectFromStream(GameMap* gameMap, std::istream& is)
{
    TreasuryObject* obj = new TreasuryObject(gameMap);
    Ogre::Vector3 position;
    Ogre::Real x, y, z;
    OD_ASSERT_TRUE(is >> x >> y >> z);
    obj->setPosition(Ogre::Vector3(x, y, z));
    OD_ASSERT_TRUE(is >> obj->mGoldValue);
    return obj;

}

TreasuryObject* TreasuryObject::getTreasuryObjectFromPacket(GameMap* gameMap, ODPacket& packet)
{
    TreasuryObject* obj = new TreasuryObject(gameMap);
    OD_ASSERT_TRUE(packet >> obj);
    return obj;
}

void TreasuryObject::exportToPacket(ODPacket& packet)
{
    packet << this;
}

std::ostream& operator<<(std::ostream& os, TreasuryObject* obj)
{
    std::string name = obj->getName();
    Ogre::Vector3 position = obj->getPosition();
    os << name;
    os << position.x;
    os << position.y;
    os << position.z;
    os << obj->mGoldValue;
    return os;
}

ODPacket& operator>>(ODPacket& is, TreasuryObject* obj)
{
    std::string name;
    OD_ASSERT_TRUE(is >> name);
    obj->setName(name);
    Ogre::Vector3 position;
    OD_ASSERT_TRUE(is >> position);
    obj->setPosition(position);
    return is;
}

ODPacket& operator<<(ODPacket& os, TreasuryObject* obj)
{
    std::string name = obj->getName();
    std::string meshName = obj->getMeshName();
    Ogre::Vector3 position = obj->getPosition();
    os << name;
    os << position;
    return os;
}

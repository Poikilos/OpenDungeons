/*
 *  Copyright (C) 2011-2016  OpenDungeons Team
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

#ifndef CREATUREEFFECTHEAL_H
#define CREATUREEFFECTHEAL_H

#include "creatureeffect/CreatureEffect.h"

class CreatureEffectHeal : public CreatureEffect
{
public:
    CreatureEffectHeal(int32_t nbTurnsEffect, double effectValue, const std::string& particleEffectName) :
        CreatureEffect(nbTurnsEffect, particleEffectName),
        mEffectValue(effectValue)
    {}

    CreatureEffectHeal() :
        CreatureEffect(),
        mEffectValue(0.0)
    {}

    virtual ~CreatureEffectHeal()
    {}

    virtual const std::string& getEffectName() const override;

    virtual void exportToStream(std::ostream& os) const override;
    virtual bool importFromStream(std::istream& is) override;

    static CreatureEffectHeal* load(std::istream& is);

protected:
    virtual void applyEffect(Creature& creature) override;

private:
    double mEffectValue;
};

#endif // CREATUREEFFECTHEAL_H

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::Rebound

Description
    Simple rebound patch interaction model

\*---------------------------------------------------------------------------*/

#ifndef Rebound_H
#define Rebound_H

#include "PatchInteractionModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
/*---------------------------------------------------------------------------*\
                           Class Rebound Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class Rebound
:
    public PatchInteractionModel<CloudType>
{
    // Private data

       //- Factor applied to velocity on rebound
       //  Normal rebound = 1
       scalar UFactor_;


public:

    //- Runtime type information
    TypeName("Rebound");


    // Constructors

        //- Construct from dictionary
        Rebound(const dictionary& dict, CloudType& cloud);


    //- Destructor
    virtual ~Rebound();


    // Member Functions

        //- Flag to indicate whether model activates patch interaction model
        virtual bool active() const;

        //- Apply velocity correction
        //  Returns true if particle remains in same cell
        virtual bool correct
        (
            const polyPatch& pp,
            const label faceId,
            bool& keepParticle,
            vector& U
        ) const;
        virtual bool correct1
        (
            typename CloudType::parcelType& p,
            const polyPatch& pp,
            const label faceId,
            bool& keepParticle,
            vector& U
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "Rebound.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //

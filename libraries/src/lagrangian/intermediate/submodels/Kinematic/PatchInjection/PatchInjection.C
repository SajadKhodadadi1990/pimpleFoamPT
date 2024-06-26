/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.1
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

\*---------------------------------------------------------------------------*/

#include "PatchInjection.H"
#include "DataEntry.H"
#include "pdf.H"
#include "label.H"
#include "Random.H"
#include "clock.H"
// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class CloudType>
Foam::label Foam::PatchInjection<CloudType>::parcelsToInject
(
    const scalar time0,
    const scalar time1
) const
{
    if ((time0 >= 0.0) && (time0 < duration_))
    {
        return round(fraction_*(time1 - time0)*parcelsPerSecond_);
    }
    else
    {
        return 0;
    }
}


template<class CloudType>
Foam::scalar Foam::PatchInjection<CloudType>::volumeToInject
(
    const scalar time0,
    const scalar time1
) const
{
    if ((time0 >= 0.0) && (time0 < duration_))
    {
        return fraction_*volumeFlowRate_().integrate(time0, time1);
    }
    else
    {
        return 0.0;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::PatchInjection<CloudType>::PatchInjection
(
    const dictionary& dict,
    CloudType& owner
)
:
    InjectionModel<CloudType>(dict, owner, typeName),
    patchName_(this->coeffDict().lookup("patchName")),
    duration_(readScalar(this->coeffDict().lookup("duration"))),
    parcelsPerSecond_
    (
        readScalar(this->coeffDict().lookup("parcelsPerSecond"))
    ),
    U0_(this->coeffDict().lookup("U0")),
    volumeFlowRate_
    (
        DataEntry<scalar>::New
        (
            "volumeFlowRate",
            this->coeffDict()
        )
    ),
    parcelPDF_
    (
        pdf::New
        (
            this->coeffDict().subDict("parcelPDF"),
            owner.rndGen()
        )
    ),
    cellOwners_(),
    fraction_(1.0)
{
    label patchId = owner.mesh().boundaryMesh().findPatchID(patchName_);

    if (patchId < 0)
    {
        FatalErrorIn
        (
            "PatchInjection<CloudType>::PatchInjection"
            "("
                "const dictionary&, "
                "CloudType&"
            ")"
        )   << "Requested patch " << patchName_ << " not found" << nl
            << "Available patches are: " << owner.mesh().boundaryMesh().names()
            << nl << exit(FatalError);
    }

    updateMesh();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::PatchInjection<CloudType>::~PatchInjection()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
bool Foam::PatchInjection<CloudType>::active() const
{
    return true;
}


template<class CloudType>
void Foam::PatchInjection<CloudType>::updateMesh()
{
    label patchId =
        this->owner().mesh().boundaryMesh().findPatchID(patchName_);

    const polyPatch& patch = this->owner().mesh().boundaryMesh()[patchId];

    cellOwners_ = patch.faceCells();

    label patchSize = cellOwners_.size();
    label totalPatchSize = patchSize;
    reduce(totalPatchSize, sumOp<label>());
    fraction_ = scalar(patchSize)/totalPatchSize;

    // Set total volume/mass to inject
    this->volumeTotal_ = fraction_*volumeFlowRate_().integrate(0.0, duration_);
    this->massTotal_ *= fraction_;
}


template<class CloudType>
Foam::scalar Foam::PatchInjection<CloudType>::timeEnd() const
{
    return this->SOI_ + duration_;
}


template<class CloudType>
void Foam::PatchInjection<CloudType>::setPositionAndCell
(
    const label parcelI,
    const label newParcels,
    const scalar time,
    vector& position,
    label& cellOwner
)
{
		     //Info<<"clock :: getTime ():"<<clock :: getTime()<<endl;

    if (cellOwners_.size() > 0)
    {
		// label KK=this->owner().rndGen().integer(0,cellOwners_.size() - 1);
		 Random ranGens1(clock :: getTime ()+pid ()+parcelI+time*1.0e06);
		 label KK=ranGens1.integer(0,this->owner().mesh().nCells());
		 Random ranGens(clock :: getTime ()+pid ()+KK);
	     label cellI=ranGens.integer(0,cellOwners_.size() - 1);
		// label cellI =ranGeneration(parcelI,0,cellOwners_.size() - 1);
      // label cellI = this->owner().rndGen().integer(0, cellOwners_.size() - 1);

         cellOwner = cellOwners_[cellI];
         position = this->owner().mesh().C()[cellOwner];
    }
    else
    {
        cellOwner = -1;
        // dummy position
        position = pTraits<vector>::max;
    }
}


template<class CloudType>
void Foam::PatchInjection<CloudType>::setProperties
(
    const label,
    const label,
    const scalar,
    typename CloudType::parcelType& parcel
)
{
    // set particle velocity
    parcel.U() = U0_;

    // set particle diameter
    parcel.d() = parcelPDF_->sample();
}


template<class CloudType>
bool Foam::PatchInjection<CloudType>::fullyDescribed() const
{
    return false;
}


template<class CloudType>
bool Foam::PatchInjection<CloudType>::validInjection(const label)
{
    return true;
}


// ************************************************************************* //

using System;
using JXClient.Domain.Coordinates;
using NUnit.Framework;

public class CalibrationEvidenceTests
{
    static readonly PlanarBounds Bounds=new PlanarBounds(-100,-100,100,100);
    static AnchorObservation Row(string label,double x,double y,double u,double v,AnchorEvidenceKind kind=AnchorEvidenceKind.MeasuredReference,string hash=null) =>
        new AnchorObservation(label,new CoordinateAnchor(new PlanarPoint(x,y),new PlanarPoint(u,v)),kind,"fixture-only.json",hash??new string('a',64));
    static AnchorObservation[] Four(AnchorEvidenceKind kind=AnchorEvidenceKind.MeasuredReference) => new[]{Row("A",0,0,10,-20,kind),Row("B",2,0,14,-20,kind),Row("C",0,4,10,-8,kind),Row("H",1,1,12,-17,kind)};
    [Test] public void CompleteConsistentMeasuredMetadataIsOnlyReadyForReview()
    { var result=CalibrationEvidence.Assess(Four(),Bounds,Bounds,.1);Assert.That(result.Kind,Is.EqualTo(CalibrationAssessmentKind.ReadyForReview));Assert.That(result.HoldoutError.Value,Is.LessThan(1e-9)); }
    [Test] public void UnknownEvidenceCannotBecomeReady()
    { Assert.That(CalibrationEvidence.Assess(Four(AnchorEvidenceKind.Unknown),Bounds,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.NeedsEvidence)); }
    [Test] public void SyntheticFixturesCannotBecomeMeasuredCalibration()
    { Assert.That(CalibrationEvidence.Assess(Four(AnchorEvidenceKind.Synthetic),Bounds,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.NeedsEvidence)); }
    [Test] public void IndependentHoldoutOutsideToleranceIsRejected()
    { var rows=Four();rows[3]=Row("H",1,1,15,-13);var result=CalibrationEvidence.Assess(rows,Bounds,Bounds,.1);Assert.That(result.Kind,Is.EqualTo(CalibrationAssessmentKind.OutOfTolerance));Assert.That(result.HoldoutError,Is.EqualTo(5).Within(1e-9)); }
    [Test] public void DuplicateHoldoutAndLabelsAreInvalid()
    { var rows=Four();rows[3]=Row("H",0,0,10,-20);Assert.That(CalibrationEvidence.Assess(rows,Bounds,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.InvalidInput));rows=Four();rows[3]=Row("A",1,1,12,-17);Assert.That(CalibrationEvidence.Assess(rows,Bounds,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.InvalidInput)); }
    [Test] public void MissingRowsAndInvalidToleranceAreInvalid()
    { foreach(double tolerance in new[]{0d,-1d,double.NaN,double.PositiveInfinity})Assert.That(CalibrationEvidence.Assess(Four(),Bounds,Bounds,tolerance).Kind,Is.EqualTo(CalibrationAssessmentKind.InvalidInput));Assert.That(CalibrationEvidence.Assess(new AnchorObservation[3],Bounds,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.InvalidInput)); }
    [Test] public void MissingOrMalformedHashesNeedEvidence()
    { var rows=Four();rows[0]=Row("A",0,0,10,-20,hash:"bad");Assert.That(CalibrationEvidence.Assess(rows,Bounds,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.NeedsEvidence)); }
    [Test] public void CollinearOrOutOfBoundsAnchorsCannotBeAssessedAsReady()
    { var rows=Four();rows[2]=Row("C",4,0,18,-20);Assert.That(CalibrationEvidence.Assess(rows,Bounds,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.InvalidInput));Assert.That(CalibrationEvidence.Assess(Four(),default,Bounds,.1).Kind,Is.EqualTo(CalibrationAssessmentKind.InvalidInput)); }
}

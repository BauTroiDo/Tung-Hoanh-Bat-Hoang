using System;
using JXClient.Domain.Coordinates;
using NUnit.Framework;

public class MapCoordinateTransformTests
{
    // All numbers are synthetic fixtures, never measured Map53 anchors.
    static readonly PlanarBounds Wide = new PlanarBounds(-100,-100,100,100);
    static CoordinateAnchor Pair(double x,double y,double u,double v) => new CoordinateAnchor(new PlanarPoint(x,y),new PlanarPoint(u,v));
    static AffineMapTransform Standard() => AffineMapTransform.Fit(Pair(0,0,10,-20),Pair(2,0,14,-20),Pair(0,4,10,-8),Wide,Wide);
    static void Equal(PlanarPoint p,double x,double y,double epsilon=1e-9) { Assert.That(p.X,Is.EqualTo(x).Within(epsilon));Assert.That(p.Y,Is.EqualTo(y).Within(epsilon)); }

    [Test] public void ScaleTranslationAndIndependentHoldoutAreCorrect()
    {
        var map=Standard();Equal(map.ToScene(new PlanarPoint(1,1)),12,-17);
        Assert.That(map.HoldoutError(Pair(1,1,12,-17)),Is.LessThan(1e-9));
        Assert.That(map.HoldoutError(Pair(1,1,15,-13)),Is.EqualTo(5).Within(1e-9));
    }
    [Test] public void RotatedAxesAndInverseUseSceneHorizontalCoordinates()
    {
        var map=AffineMapTransform.Fit(Pair(0,0,10,-20),Pair(2,0,10,-14),Pair(0,4,2,-20),Wide,Wide);
        Equal(map.ToScene(new PlanarPoint(1,1)),8,-17);Equal(map.ToLegacy(new PlanarPoint(8,-17)),1,1);
    }
    [Test] public void ReflectionAndNegativeCoordinatesRoundTrip()
    {
        var map=AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,-2,0),Pair(0,1,0,3),Wide,Wide);
        Equal(map.ToScene(new PlanarPoint(-4,-5)),8,-15);Equal(map.ToLegacy(new PlanarPoint(8,-15)),-4,-5);
    }
    [Test] public void LargeSourceOriginPreservesLocalDisplacement()
    {
        var source=new PlanarBounds(1e12,1e12,1e12+100,1e12+100);
        var map=AffineMapTransform.Fit(Pair(1e12,1e12,0,0),Pair(1e12+2,1e12,4,0),Pair(1e12,1e12+4,0,12),source,Wide);
        Equal(map.ToScene(new PlanarPoint(1e12+.25,1e12+.5)),.5,1.5);
        Equal(map.ToLegacy(new PlanarPoint(.5,1.5)),1e12+.25,1e12+.5);
    }
    [Test] public void BoundsAreInclusiveAndBothDomainsAreChecked()
    {
        var b=new PlanarBounds(0,0,10,10);
        var map=AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,1,0),Pair(0,1,0,1),b,b);
        Equal(map.ToScene(new PlanarPoint(10,10)),10,10);
        Assert.Throws<ArgumentOutOfRangeException>(()=>map.ToScene(new PlanarPoint(11,0)));
        Assert.Throws<ArgumentOutOfRangeException>(()=>map.ToLegacy(new PlanarPoint(-1,0)));
        var clipped=AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,2,0),Pair(0,1,0,2),b,b);
        Assert.Throws<ArgumentOutOfRangeException>(()=>clipped.ToScene(new PlanarPoint(6,0)));
        var inverseClipped=AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,.5,0),Pair(0,1,0,.5),b,b);
        Assert.Throws<ArgumentOutOfRangeException>(()=>inverseClipped.ToLegacy(new PlanarPoint(6,0)));
    }
    [Test] public void DuplicateAndCollinearSourceAnchorsAreRejected()
    {
        Assert.Throws<ArgumentException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(0,0,1,0),Pair(0,1,0,1),Wide,Wide));
        Assert.Throws<ArgumentException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,1,1,0),Pair(2,2,0,1),Wide,Wide));
    }
    [Test] public void NearCollinearSourceAnchorsAreRejectedRelatively()
    {
        Assert.Throws<ArgumentException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,1,0),Pair(1,1e-14,0,1),Wide,Wide));
    }
    [Test] public void NonInvertibleSceneMappingIsRejected()
    {
        Assert.Throws<ArgumentException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,1,1),Pair(0,1,2,2),Wide,Wide));
        Assert.Throws<ArgumentException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,1,0),Pair(0,1,1,1e-14),Wide,Wide));
    }
    [Test] public void NonFiniteValuesAndInvalidBoundsAreRejected()
    {
        Assert.Throws<ArgumentException>(()=>new PlanarPoint(double.NaN,0));
        Assert.Throws<ArgumentException>(()=>new PlanarPoint(0,double.PositiveInfinity));
        Assert.Throws<ArgumentException>(()=>new PlanarBounds(2,0,1,1));
        Assert.Throws<ArgumentException>(()=>new PlanarBounds(0,0,0,1));
        Assert.Throws<ArgumentException>(()=>new PlanarBounds(0,0,double.PositiveInfinity,1));
        Assert.Throws<ArgumentException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,1,0),Pair(0,1,0,1),default,Wide));
        var extreme=new PlanarBounds(-double.MaxValue,-double.MaxValue,double.MaxValue,double.MaxValue);
        Assert.Throws<ArgumentException>(()=>AffineMapTransform.Fit(Pair(-double.MaxValue,0,0,0),Pair(double.MaxValue,0,1,0),Pair(0,1,0,1),extreme,Wide));
    }
    [Test] public void CalibrationAnchorsOutsideBoundsAreRejected()
    {
        var small=new PlanarBounds(0,0,1,1);
        Assert.Throws<ArgumentOutOfRangeException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(2,0,1,0),Pair(0,1,0,1),small,Wide));
        Assert.Throws<ArgumentOutOfRangeException>(()=>AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,2,0),Pair(0,1,0,1),Wide,small));
    }
    [Test] public void IdentityFitAndInverseAgreeAcrossBounds()
    {
        var map=AffineMapTransform.Fit(Pair(0,0,0,0),Pair(1,0,1,0),Pair(0,1,0,1),Wide,Wide);
        foreach(var point in new[]{new PlanarPoint(-100,-100),new PlanarPoint(-3.5,7.25),new PlanarPoint(100,100)}) {
            Equal(map.ToScene(point),point.X,point.Y);Equal(map.ToLegacy(point),point.X,point.Y);
        }
    }
}

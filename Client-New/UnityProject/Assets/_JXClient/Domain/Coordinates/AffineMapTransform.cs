using System;

namespace JXClient.Domain.Coordinates
{
    /// <summary>A finite horizontal pair. Scene Y here means planar Z, never terrain height.</summary>
    public readonly struct PlanarPoint
    {
        public double X { get; }
        public double Y { get; }
        public PlanarPoint(double x, double y)
        {
            if (!Finite(x) || !Finite(y)) throw new ArgumentException("Coordinates must be finite.");
            X = x; Y = y;
        }
        internal static bool Finite(double value) => !double.IsNaN(value) && !double.IsInfinity(value);
    }

    public readonly struct PlanarBounds
    {
        public double MinX { get; }
        public double MinY { get; }
        public double MaxX { get; }
        public double MaxY { get; }
        public bool IsValid => PlanarPoint.Finite(MinX) && PlanarPoint.Finite(MinY) &&
            PlanarPoint.Finite(MaxX) && PlanarPoint.Finite(MaxY) && MinX < MaxX && MinY < MaxY;
        public PlanarBounds(double minX, double minY, double maxX, double maxY)
        {
            MinX = minX; MinY = minY; MaxX = maxX; MaxY = maxY;
            if (!IsValid) throw new ArgumentException("Bounds must be finite, ordered and non-empty.");
        }
        public bool Contains(PlanarPoint point) => IsValid && point.X >= MinX && point.X <= MaxX &&
            point.Y >= MinY && point.Y <= MaxY;
        internal void Require(PlanarPoint point)
        {
            if (!Contains(point)) throw new ArgumentOutOfRangeException(nameof(point), "Coordinate outside configured domain.");
        }
    }

    public readonly struct CoordinateAnchor
    {
        public PlanarPoint Legacy { get; }
        public PlanarPoint Scene { get; }
        public CoordinateAnchor(PlanarPoint legacy, PlanarPoint scene) { Legacy = legacy; Scene = scene; }
    }

    /// <summary>
    /// Pure numerical mapping, not proof of map calibration, provenance or server authority.
    /// Fit is origin-relative to preserve local detail near large legacy coordinates.
    /// </summary>
    public sealed class AffineMapTransform
    {
        const double RelativeDeterminantFloor = 1e-12;
        readonly PlanarPoint legacyOrigin, sceneOrigin;
        readonly double a, b, c, d, ia, ib, ic, id;
        public PlanarBounds LegacyBounds { get; }
        public PlanarBounds SceneBounds { get; }

        AffineMapTransform(CoordinateAnchor origin, double a, double b, double c, double d,
            PlanarBounds legacyBounds, PlanarBounds sceneBounds)
        {
            this.a = Checked(a); this.b = Checked(b); this.c = Checked(c); this.d = Checked(d);
            legacyOrigin = origin.Legacy; sceneOrigin = origin.Scene;
            LegacyBounds = legacyBounds; SceneBounds = sceneBounds;
            double scale = MaxAbs(a, b, c, d);
            if (scale == 0) throw new ArgumentException("Scene transform is singular.");
            double na = a / scale, nb = b / scale, nc = c / scale, nd = d / scale;
            double det = na * nd - nb * nc;
            if (Math.Abs(det) <= RelativeDeterminantFloor) throw new ArgumentException("Scene transform is singular or poorly conditioned.");
            ia = Checked(nd / det / scale); ib = Checked(-nb / det / scale);
            ic = Checked(-nc / det / scale); id = Checked(na / det / scale);
        }

        public static AffineMapTransform Fit(CoordinateAnchor first, CoordinateAnchor second,
            CoordinateAnchor third, PlanarBounds legacyBounds, PlanarBounds sceneBounds)
        {
            if (!legacyBounds.IsValid || !sceneBounds.IsValid) throw new ArgumentException("Valid bounds are required.");
            foreach (var anchor in new[] { first, second, third })
            { legacyBounds.Require(anchor.Legacy); sceneBounds.Require(anchor.Scene); }
            double x1 = Checked(second.Legacy.X - first.Legacy.X), y1 = Checked(second.Legacy.Y - first.Legacy.Y);
            double x2 = Checked(third.Legacy.X - first.Legacy.X), y2 = Checked(third.Legacy.Y - first.Legacy.Y);
            double scale = MaxAbs(x1, y1, x2, y2);
            if (scale == 0) throw new ArgumentException("Source anchors must be distinct.");
            x1 /= scale; y1 /= scale; x2 /= scale; y2 /= scale;
            double det = x1 * y2 - x2 * y1;
            if (Math.Abs(det) <= RelativeDeterminantFloor) throw new ArgumentException("Source anchors are collinear or poorly conditioned.");
            SolveRow(Checked(second.Scene.X - first.Scene.X), Checked(third.Scene.X - first.Scene.X),
                x1, y1, x2, y2, det, scale, out double a, out double b);
            SolveRow(Checked(second.Scene.Y - first.Scene.Y), Checked(third.Scene.Y - first.Scene.Y),
                x1, y1, x2, y2, det, scale, out double c, out double d);
            return new AffineMapTransform(first, a, b, c, d, legacyBounds, sceneBounds);
        }

        static void SolveRow(double u1, double u2, double x1, double y1, double x2, double y2,
            double det, double sourceScale, out double first, out double second)
        {
            double targetScale = Math.Max(Math.Abs(u1), Math.Abs(u2));
            if (targetScale == 0) { first = 0; second = 0; return; }
            u1 /= targetScale; u2 /= targetScale;
            double ratio = Checked(targetScale / sourceScale);
            first = Checked((u1 * y2 - u2 * y1) / det * ratio);
            second = Checked((x1 * u2 - x2 * u1) / det * ratio);
        }

        public PlanarPoint ToScene(PlanarPoint legacy)
        {
            LegacyBounds.Require(legacy);
            double x = Checked(legacy.X - legacyOrigin.X), y = Checked(legacy.Y - legacyOrigin.Y);
            var scene = new PlanarPoint(sceneOrigin.X + a * x + b * y, sceneOrigin.Y + c * x + d * y);
            SceneBounds.Require(scene); return scene;
        }

        public PlanarPoint ToLegacy(PlanarPoint scene)
        {
            SceneBounds.Require(scene);
            double x = Checked(scene.X - sceneOrigin.X), y = Checked(scene.Y - sceneOrigin.Y);
            var legacy = new PlanarPoint(legacyOrigin.X + ia * x + ib * y, legacyOrigin.Y + ic * x + id * y);
            LegacyBounds.Require(legacy); return legacy;
        }

        public double HoldoutError(CoordinateAnchor holdout)
        {
            SceneBounds.Require(holdout.Scene);
            var predicted = ToScene(holdout.Legacy);
            double dx = Math.Abs(Checked(predicted.X - holdout.Scene.X));
            double dy = Math.Abs(Checked(predicted.Y - holdout.Scene.Y));
            double scale = Math.Max(dx, dy);
            if (scale == 0) return 0;
            return Checked(scale * Math.Sqrt((dx / scale) * (dx / scale) + (dy / scale) * (dy / scale)));
        }

        static double Checked(double value)
        {
            if (!PlanarPoint.Finite(value)) throw new ArgumentException("Coordinate arithmetic exceeded finite double range.");
            return value;
        }
        static double MaxAbs(double a, double b, double c, double d) =>
            Math.Max(Math.Max(Math.Abs(a), Math.Abs(b)), Math.Max(Math.Abs(c), Math.Abs(d)));
    }
}

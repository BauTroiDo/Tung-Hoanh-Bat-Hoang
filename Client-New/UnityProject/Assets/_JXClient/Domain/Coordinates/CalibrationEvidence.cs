using System;
using System.Collections.Generic;

namespace JXClient.Domain.Coordinates
{
    public enum AnchorEvidenceKind { Unknown, Synthetic, MeasuredReference }
    // Numerical/structural assessment only. No Verified state or map-authority side effect exists.
    public enum CalibrationAssessmentKind { InvalidInput, NeedsEvidence, OutOfTolerance, ReadyForReview }

    public sealed class AnchorObservation
    {
        public string Label { get; }
        public CoordinateAnchor Pair { get; }
        public AnchorEvidenceKind Kind { get; }
        public string Reference { get; }
        public string SHA256 { get; }
        public AnchorObservation(string label, CoordinateAnchor pair, AnchorEvidenceKind kind, string reference, string sha256)
        {
            Label = label?.Trim() ?? string.Empty; Pair = pair; Kind = kind;
            Reference = reference?.Trim() ?? string.Empty; SHA256 = sha256?.Trim() ?? string.Empty;
        }
    }
    public sealed class CalibrationAssessment
    {
        public CalibrationAssessmentKind Kind { get; }
        public string Message { get; }
        public double? HoldoutError { get; }
        internal CalibrationAssessment(CalibrationAssessmentKind kind, string message, double? error)
        { Kind = kind; Message = message; HoldoutError = error; }
    }
    public static class CalibrationEvidence
    {
        public static CalibrationAssessment Assess(IReadOnlyList<AnchorObservation> rows, PlanarBounds legacy,
            PlanarBounds scene, double tolerance)
        {
            if (rows == null || rows.Count != 4 || !PlanarPoint.Finite(tolerance) || tolerance <= 0)
                return Invalid("Exactly three fit anchors, one holdout and an explicit positive finite tolerance are required.");
            var labels = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            bool measured = true;
            for (int i = 0; i < rows.Count; i++)
            {
                var row = rows[i];
                if (row == null || string.IsNullOrWhiteSpace(row.Label) || !labels.Add(row.Label))
                    return Invalid("Four distinct named observations are required.");
                for (int j = 0; j < i; j++)
                    if (row.Pair.Legacy.X == rows[j].Pair.Legacy.X && row.Pair.Legacy.Y == rows[j].Pair.Legacy.Y)
                        return Invalid("Holdout and fit observations must use distinct legacy locations.");
                measured &= row.Kind == AnchorEvidenceKind.MeasuredReference &&
                    !string.IsNullOrWhiteSpace(row.Reference) && IsSha256(row.SHA256);
            }
            try
            {
                var map = AffineMapTransform.Fit(rows[0].Pair, rows[1].Pair, rows[2].Pair, legacy, scene);
                double error = map.HoldoutError(rows[3].Pair);
                if (!measured) return new CalibrationAssessment(CalibrationAssessmentKind.NeedsEvidence,
                    "All four observations need measured-reference provenance and SHA256 evidence. Synthetic fixtures do not establish calibration.", error);
                if (error > tolerance) return new CalibrationAssessment(CalibrationAssessmentKind.OutOfTolerance,
                    "Independent holdout residual exceeds the declared scene-unit tolerance.", error);
                return new CalibrationAssessment(CalibrationAssessmentKind.ReadyForReview,
                    "Candidate passes numerical and metadata checks; independent evidence review is still required.", error);
            }
            catch (ArgumentException ex) { return Invalid(ex.Message); }
        }
        static CalibrationAssessment Invalid(string message) => new CalibrationAssessment(CalibrationAssessmentKind.InvalidInput, message, null);
        static bool IsSha256(string text)
        {
            if (text == null || text.Length != 64) return false;
            foreach (char c in text)
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) return false;
            return true;
        }
    }
}

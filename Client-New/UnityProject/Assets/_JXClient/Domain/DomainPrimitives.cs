using System;

namespace JXClient.Domain
{
    /// <summary>
    /// Pure C# domain value types with strict invariants (no UnityEngine references).
    /// </summary>
    public readonly struct PlayerId : IEquatable<PlayerId>
    {
        public int Value { get; }

        public PlayerId(int value)
        {
            if (value <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(value), "PlayerId must be a positive integer.");
            }

            Value = value;
        }

        public bool IsValid => Value > 0;

        public bool Equals(PlayerId other) => Value == other.Value;
        public override bool Equals(object obj) => obj is PlayerId other && Equals(other);
        public override int GetHashCode() => Value;
        public override string ToString() => $"Player({Value})";
    }

    public readonly struct MapId : IEquatable<MapId>
    {
        public static readonly MapId BaLangHuyen = new MapId(53);

        public int Value { get; }

        public MapId(int value)
        {
            if (value <= 0 || value > 65535)
            {
                throw new ArgumentOutOfRangeException(nameof(value), "MapId must be within [1..65535].");
            }

            Value = value;
        }

        public bool IsValid => Value > 0 && Value <= 65535;

        public bool Equals(MapId other) => Value == other.Value;
        public override bool Equals(object obj) => obj is MapId other && Equals(other);
        public override int GetHashCode() => Value;
        public override string ToString() => $"Map({Value})";
    }

    public readonly struct WorldPosition : IEquatable<WorldPosition>
    {
        public float X { get; }
        public float Y { get; }
        public float Z { get; }

        public WorldPosition(float x, float y, float z)
        {
            if (float.IsNaN(x) || float.IsInfinity(x) ||
                float.IsNaN(y) || float.IsInfinity(y) ||
                float.IsNaN(z) || float.IsInfinity(z))
            {
                throw new ArgumentException("WorldPosition coordinates must be finite numbers.");
            }

            X = x;
            Y = y;
            Z = z;
        }

        public bool Equals(WorldPosition other) => X.Equals(other.X) && Y.Equals(other.Y) && Z.Equals(other.Z);
        public override bool Equals(object obj) => obj is WorldPosition other && Equals(other);
        public override int GetHashCode() => HashCode.Combine(X, Y, Z);
        public override string ToString() => $"({X:F2}, {Y:F2}, {Z:F2})";
    }

    public readonly struct MoveIntent
    {
        public uint Sequence { get; }
        public uint MapEpoch { get; }
        public WorldPosition Destination { get; }

        public MoveIntent(uint sequence, uint mapEpoch, WorldPosition destination)
        {
            if (sequence == 0)
            {
                throw new ArgumentOutOfRangeException(nameof(sequence), "MoveIntent sequence must be greater than zero.");
            }

            if (mapEpoch == 0)
            {
                throw new ArgumentOutOfRangeException(nameof(mapEpoch), "MoveIntent mapEpoch must be greater than zero.");
            }

            Sequence = sequence;
            MapEpoch = mapEpoch;
            Destination = destination;
        }
    }
}

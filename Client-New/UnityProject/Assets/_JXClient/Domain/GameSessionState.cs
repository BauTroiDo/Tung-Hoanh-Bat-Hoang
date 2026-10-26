namespace JXClient.Domain
{
    /// <summary>
    /// Mandatory session state machine defined in docs/01_ARCHITECTURE.md.
    /// MoveRequest is strictly forbidden prior to WorldReady.
    /// </summary>
    public enum GameSessionState
    {
        AppBoot = 0,
        OfflineReady = 1,
        Login = 2,
        Authenticating = 3,
        Roles = 4,
        JoiningWorld = 5,
        LoadingScene = 6,
        AwaitingSnapshot = 7,
        PlayerSpawned = 8,
        WorldReady = 9,
        Reconnecting = 10,
        Disconnected = 11
    }

    public static class GameSessionRules
    {
        public static bool CanSendMoveRequest(GameSessionState state)
        {
            return state == GameSessionState.WorldReady;
        }
    }
}

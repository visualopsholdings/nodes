const PROXY_CONFIG = [
    {
        context: [
            "/login",
            "/rest",
            "/fonts",
            "/socket.io"
        ],
        target: "http://localhost:4000",
        secure: false,
        logLevel: "debug"
    }
]

module.exports = PROXY_CONFIG;

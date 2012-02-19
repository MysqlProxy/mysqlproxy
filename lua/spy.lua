-- create spy table
spy = {}

spy.core = {
    listen_addr = "127.0.0.1:1919|0.0.0.0:1919" -- spy listen addr
    worker_connections = 300, -- max connection for each work process
    error_log = "logs/error.log", -- error log path
    error_level = "notice", -- error log level
    debug_level = "core|event" -- error debug level , "|" is a sep for multi level
}

-- spy databases
spy.db = {
    -- [mysql:]username[:password]@hostname[:port]/ dbname
    db01 = {
       conn = "test:test@10.20.139.147:3306/test",
       min_psize = 100,
       max_psize = 500
    },

    db02 = {
        conn = "test1:test1@10.20.139.147:3306/test",
        min_psize = 100,
        max_psize = 500
    }
}

-- spy login
spy.login = {
    -- user : test
    test = {
        password = "test",

        wrdb = {
            spy.db["db01"]
        },

        rdb = {
            spy.db["db02"]
        }
    }, 

    -- user : test1
    test1 = {
        password = "test1"
    }
} 



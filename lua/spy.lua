-- create spy table
spy = {}

spy.core = {
    port = 1919,
    listen = {
        "127.0.0.1"
    },
    timeout = 20
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




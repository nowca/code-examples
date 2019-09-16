// server base object
var PIMServer = {
    config:             require('../config/config'),
    modules: {
        Express:        require('express'),
	    HTTPS:		    require('https'),
        Sequelize:      require("sequelize"),
        BodyParser:     require('body-parser'),
        Cors:           require('cors'),
        Multer:         require('multer'),
        FS:             require('fs'),
        Crypto:         require('crypto'),
        AuthController: null,
        JWT:            require('jsonwebtoken'),
        Passport:       require('passport'),
        PassportJWT:    require('passport-jwt') 
    },
    print: {
        success: function (title, message) {
            console.log('\033[32m' + (message ? '[' : '') + title + (message ? '] ' : '') + '\033[0m' + message);
        },
        error: function (title, message) {
            console.log('\033[31m' + (message ? '[' : '') + title + (message ? '] ' : '') + '\033[0m' + message);
        }
    }
};

// server base modules
PIMServer.app = PIMServer.modules.Express();
PIMServer.app.use(PIMServer.modules.Cors());
PIMServer.app.use(PIMServer.modules.BodyParser.json());
PIMServer.app.use(PIMServer.modules.BodyParser.urlencoded({extended: true}));
PIMServer.app.use(PIMServer.modules.Passport.initialize())

PIMServer.router = PIMServer.modules.Express.Router();
PIMServer.orm    = new PIMServer.modules.Sequelize(
    PIMServer.config.db.name,
    PIMServer.config.db.user.name,
    PIMServer.config.db.user.password,
    {
        dialect:  PIMServer.config.db.dialect,
        host:     PIMServer.config.db.host,
        port:     PIMServer.config.db.port,
        logging: function (message) {
            PIMServer.print.success('Database', message)
        },
        define: {
            timestamps: false
        }
    }
);

// print start info
PIMServer.print.success('Server', 'PIMServer running.');

// test db connection
PIMServer.orm.authenticate().then(function (err) {
    PIMServer.print.success('Database', 'Connection established:' + PIMServer.config.db.user.name + '@' + PIMServer.config.db.host);
}, function (err) {
    PIMServer.print.error(err.name, err.message);
});

// run RESTful webservice
require('../controller/router')(PIMServer);

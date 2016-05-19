CREATE TABLE version (
    version TEXT NOT NULL UNIQUE
);

INSERT INTO version VALUES ('v1.0');

CREATE TABLE part_unit  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    abbreviation TEXT NOT NULL UNIQUE,
    defaultUnit BOOLEAN
);

CREATE TABLE storage  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT ,
    lft INTEGER NOT NULL,
    rgt INTEGER NOT NULL
);

CREATE INDEX storage_lft_idx on storage (lft);
CREATE INDEX storage_rgt_idx on storage (rgt);

CREATE TABLE category  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT ,
    lft INTEGER NOT NULL,
    rgt INTEGER NOT NULL
);

CREATE INDEX cat_lft_idx on category (lft);
CREATE INDEX cat_rgt_idx on category (rgt);


CREATE TABLE condition  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    value TEXT NOT NULL,
    description TEXT ,
    defaultCondition BOOLEAN
);

CREATE TABLE package  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT ,
    imageFilename TEXT
);

CREATE TABLE part  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT ,
    actualStock INTEGER ,
    minimumStock INTEGER ,
    averagePrice DOUBLE ,
    comment TEXT ,
    customPartNumber TEXT ,
    createDate DATETIME NOT NULL,
    partUnit INTEGER NOT NULL,
    category INTEGER NOT NULL,
    storage INTEGER NOT NULL,
    condition INTEGER ,
    package INTEGER ,
    FOREIGN KEY(partUnit) REFERENCES part_unit(id),
    FOREIGN KEY(category) REFERENCES category(id),
    FOREIGN KEY(storage) REFERENCES storage(id),
    FOREIGN KEY(condition) REFERENCES condition(id),
    FOREIGN KEY(package) REFERENCES package(id)
);

CREATE TABLE stock_change  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    change INTEGER NOT NULL,
    price DOUBLE ,
    dateTime DATETIME NOT NULL,
    comment TEXT ,
    part INTEGER NOT NULL,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE
);

CREATE TABLE unit  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    symbol TEXT ,
    deletable BOOLEAN NOT NULL
);

CREATE TABLE datatype  (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE parameter  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    key TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    datatype INTEGER ,
    deletable BOOLEAN NOT NULL,
    unit INTEGER ,
    description TEXT ,
    FOREIGN KEY(datatype) REFERENCES datatype(id),
    FOREIGN KEY(unit) REFERENCES unit(id)
);

CREATE TABLE part_parameter  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    parameter INTEGER NOT NULL,
    numericValue DOUBLE ,
    textValue TEXT ,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,
    FOREIGN KEY(parameter) REFERENCES parameter(id)
);

CREATE TABLE distributor  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    website TEXT ,
    comment TEXT
);

CREATE TABLE packaging  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL
);

CREATE TABLE part_distributor  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    distributor INTEGER NOT NULL,
    partNumber TEXT ,
    minimumOrder INTEGER ,
    unitPrice DOUBLE ,
    packaging INTEGER ,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,
    FOREIGN KEY(distributor) REFERENCES distributor(id) ON DELETE CASCADE,
    FOREIGN KEY(packaging) REFERENCES packaging(id) ON DELETE CASCADE
);

CREATE TABLE manufacturer  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    website TEXT ,
    comment TEXT
);

CREATE TABLE part_manufacturer  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    manufacturer INTEGER NOT NULL,
    partNumber TEXT NOT NULL,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,
    FOREIGN KEY(manufacturer) REFERENCES manufacturer(id) ON DELETE CASCADE
);

CREATE TABLE part_attachment  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    url TEXT NOT NULL,
    description TEXT ,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE
);

CREATE TABLE package_attachment  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    package INTEGER NOT NULL,
    url TEXT NOT NULL,
    description TEXT ,
    FOREIGN KEY(package) REFERENCES package(id) ON DELETE CASCADE
);

PRAGMA foreign_keys = ON

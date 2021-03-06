CREATE TABLE IF NOT EXISTS db_version (version TEXT NOT NULL UNIQUE);

INSERT INTO db_version VALUES ('v1.0');

CREATE TABLE IF NOT EXISTS part_unit  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    abbreviation TEXT NOT NULL UNIQUE,
    defaultUnit BOOLEAN
);

CREATE TABLE IF NOT EXISTS storage  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    icon TEXT,
    lft INTEGER NOT NULL,
    rgt INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS storage_lft_idx on storage (lft);
CREATE INDEX IF NOT EXISTS storage_rgt_idx on storage (rgt);

CREATE TABLE IF NOT EXISTS category  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    icon TEXT,
    lft INTEGER NOT NULL,
    rgt INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS cat_lft_idx on category (lft);
CREATE INDEX IF NOT EXISTS cat_rgt_idx on category (rgt);

CREATE TABLE IF NOT EXISTS condition  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    value TEXT NOT NULL,
    description TEXT ,
    defaultCondition BOOLEAN
);

CREATE TABLE IF NOT EXISTS package  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT ,
    imageFilename TEXT
);

CREATE TABLE IF NOT EXISTS part  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT ,
    totalStock INTEGER ,
    minimumStock INTEGER ,
    averagePrice DOUBLE ,
    comment TEXT ,
    customPartNumber TEXT ,
    createDate DATETIME NOT NULL,
    partUnit INTEGER NOT NULL,
    category INTEGER NOT NULL,
    package INTEGER ,
    FOREIGN KEY(partUnit) REFERENCES part_unit(id),
    FOREIGN KEY(category) REFERENCES category(id),    
    FOREIGN KEY(package) REFERENCES package(id)
);

CREATE TABLE IF NOT EXISTS part_stock (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    quantity INTEGER NOT NULL,
    storage INTEGER NOT NULL,
    condition INTEGER NOT NULL,
    lastUpdate DATETIME NOT NULL,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,
    FOREIGN KEY(storage) REFERENCES storage(id),
    FOREIGN KEY(condition) REFERENCES condition(id)
);

CREATE UNIQUE INDEX IF NOT EXISTS part_stock_idx on part_stock (part, storage, condition);

CREATE TABLE IF NOT EXISTS part_stock_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    change DOUBLE NOT NULL,
    price DOUBLE,
    lastUpdate DATETIME NOT NULL,
    comment TEXT,
    part INTEGER NOT NULL,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS unit  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    symbol TEXT ,
    deletable BOOLEAN NOT NULL
);

CREATE TABLE IF NOT EXISTS datatype  (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS parameter  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    key TEXT UNIQUE,
    name TEXT NOT NULL,
    datatype INTEGER NOT NULL,
    deletable BOOLEAN NOT NULL,
    unit INTEGER ,
    description TEXT ,
    FOREIGN KEY(datatype) REFERENCES datatype(id),
    FOREIGN KEY(unit) REFERENCES unit(id)
);

CREATE TABLE IF NOT EXISTS part_parameter  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    parameter INTEGER NOT NULL,
    value NUMERIC,
    minValue NUMERIC,
    maxValue NUMERIC,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,
    FOREIGN KEY(parameter) REFERENCES parameter(id)
);

CREATE TABLE IF NOT EXISTS distributor  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    website TEXT ,
    comment TEXT
);

CREATE TABLE IF NOT EXISTS packaging  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS part_distributor  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    distributor INTEGER NOT NULL,
    partNumber TEXT ,
    minimumOrder DOUBLE ,
    unitPrice DOUBLE ,
    currency TEXT,
    packaging INTEGER ,
    dateTime DATETIME NOT NULL,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,
    FOREIGN KEY(distributor) REFERENCES distributor(id) ON DELETE CASCADE,
    FOREIGN KEY(packaging) REFERENCES packaging(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS manufacturer  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    website TEXT ,
    comment TEXT
);

CREATE TABLE IF NOT EXISTS part_manufacturer  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    manufacturer INTEGER NOT NULL,
    partNumber TEXT NOT NULL,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,
    FOREIGN KEY(manufacturer) REFERENCES manufacturer(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS part_attachment  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    part INTEGER NOT NULL,
    url TEXT NOT NULL,
    description TEXT ,
    FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS package_attachment  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    package INTEGER NOT NULL,
    url TEXT NOT NULL,
    description TEXT ,
    FOREIGN KEY(package) REFERENCES package(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS project  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT ,
    remark TEXT
);

CREATE TABLE IF NOT EXISTS project_attachment  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project INTEGER NOT NULL,
    url TEXT NOT NULL,
    description TEXT ,
    FOREIGN KEY(project) REFERENCES project(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS project_part (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project INTEGER NOT NULL,
    quantity INTEGER NOT NULL,
    refdes TEXT NOT NULL,
    part_name TEXT NULL,
    assigned_part INTEGER NULL,
    remark TEXT ,
    FOREIGN KEY(project) REFERENCES project(id) ON DELETE CASCADE,
    FOREIGN KEY(assigned_part) REFERENCES part(id)
);

CREATE TABLE IF NOT EXISTS octopart_distributor (
    octopart_uid TEXT PRIMARY KEY,
    distributor INTEGER NOT NULL,
    FOREIGN KEY(distributor) REFERENCES distributor(id) ON DELETE CASCADE,
);

CREATE TABLE IF NOT EXISTS octopart_manufacturer (
    octopart_uid TEXT PRIMARY KEY,
    (manufacturer) INTEGER NOT NULL,
    FOREIGN KEY(manufacturer) REFERENCES manufacturer(id) ON DELETE CASCADE
);


PRAGMA foreign_keys = ON

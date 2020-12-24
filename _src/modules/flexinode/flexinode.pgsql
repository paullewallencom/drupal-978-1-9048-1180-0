-- $Id: flexinode.pgsql,v 1.2 2005/07/25 16:06:57 cvbge Exp $

CREATE TABLE flexinode_field (
  field_id int NOT NULL default '0',
  ctype_id int NOT NULL default '0',
  label varchar(255) NOT NULL default '',
  default_value text NOT NULL default '',
  weight int NOT NULL default '0',
  required int NOT NULL default '0',
  show_teaser int NOT NULL default '0',
  show_table int NOT NULL default '0',
  rows int NOT NULL default '0',
  field_type varchar(255) NOT NULL default '',
  options text NOT NULL default '',
  description varchar(255) NOT NULL default '',
  PRIMARY KEY (field_id)
);
CREATE INDEX flexinode_field_ctype_id ON flexinode_field(ctype_id);

CREATE TABLE flexinode_type (
  ctype_id int NOT NULL default '0',
  name varchar(255) NOT NULL default '',
  description varchar(255) NOT NULL default '',
  help text NOT NULL default '',
  PRIMARY KEY (ctype_id)
);

CREATE TABLE flexinode_data (
  nid int NOT NULL default '0',
  field_id int NOT NULL default '0',
  textual_data text NOT NULL default '',
  numeric_data int NOT NULL default '0',
  serialized_data text NOT NULL default '',
  PRIMARY KEY (nid,field_id)
);

CREATE SEQUENCE flexinode_ctype_seq INCREMENT 1 START 1;
CREATE SEQUENCE flexinode_field_seq INCREMENT 1 START 1;

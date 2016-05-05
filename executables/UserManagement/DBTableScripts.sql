BEGIN;

CREATE TABLE if not exists permission 
(
  id serial NOT NULL,
  parentid integer DEFAULT (-1),
  name character varying(64) NOT NULL,
  title character varying(64) DEFAULT NULL::character varying,
  description character varying(1024) DEFAULT NULL::character varying,
  CONSTRAINT permission_pkey PRIMARY KEY (id),
  CONSTRAINT permission_name_key UNIQUE (name)
)
WITH (
  OIDS=FALSE
);


CREATE TABLE if not exists users
(
  id serial NOT NULL,
  username character varying(32),
  password character varying(255),
  name character varying(255),
  banned boolean DEFAULT false,
  banreason character varying(255) DEFAULT NULL::character varying,
  avatar character varying(255) DEFAULT NULL::character varying,
  CONSTRAINT users_pkey PRIMARY KEY (id),
  CONSTRAINT users_username_key UNIQUE (username)
)
WITH (
  OIDS=FALSE
);


CREATE TABLE if not exists groups
(
  id serial NOT NULL,
  title character varying(64),
  parentid integer DEFAULT (-1),
  description character varying(4096) DEFAULT NULL::character varying,
  CONSTRAINT groups_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);


CREATE TABLE if not exists grouppermission
(
  permissionid integer,
  groupid integer,
  state integer DEFAULT 0,
  CONSTRAINT grouppermission_groupid_fkey FOREIGN KEY (groupid)
      REFERENCES groups (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT grouppermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES permission (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (
  OIDS=FALSE
);


CREATE TABLE if not exists servicepermission
(
  id serial NOT NULL,
  service character varying(64),
  permissionid integer,
  CONSTRAINT servicepermission_pkey PRIMARY KEY (id),
  CONSTRAINT servicepermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES permission (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (
  OIDS=FALSE
);


CREATE TABLE if not exists usergroup
(
  userid integer,
  groupid integer,
  CONSTRAINT usergroup_groupid_fkey FOREIGN KEY (groupid)
      REFERENCES groups (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT usergroup_userid_fkey FOREIGN KEY (userid)
      REFERENCES users (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (
  OIDS=FALSE
);


CREATE TABLE if not exists userpermission
(
  userid integer,
  permissionid integer,
  state integer DEFAULT 0,
  CONSTRAINT userpermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES permission (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT userpermission_userid_fkey FOREIGN KEY (userid)
      REFERENCES users (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (
  OIDS=FALSE
);

INSERT INTO permission(id,name,parentid,name,title) values(0,-1,'_masterPermission','Permissions');

END;
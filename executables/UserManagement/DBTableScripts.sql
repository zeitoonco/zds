BEGIN;

CREATE TABLE if not exists "UserManagement".permission
(
  id serial NOT NULL,
  parentid integer,
  name character varying(64) NOT NULL,
  title character varying(64) DEFAULT NULL::character varying,
  description character varying(1024) DEFAULT NULL::character varying,
  CONSTRAINT permission_pkey PRIMARY KEY (id),
  CONSTRAINT permission_parentid_fkey FOREIGN KEY (parentid)
      REFERENCES "UserManagement".permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT permission_name_key UNIQUE (name)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "UserManagement".permission
  OWNER TO "UserManagement";

CREATE TABLE if not exists "UserManagement".users
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
ALTER TABLE "UserManagement".users
  OWNER TO "UserManagement";

CREATE TABLE if not exists "UserManagement".groups
(
  id serial NOT NULL,
  title character varying(64),
  parentid integer,
  description character varying(4096) DEFAULT NULL::character varying,
  CONSTRAINT groups_pkey PRIMARY KEY (id),
  CONSTRAINT groups_parentid_fkey FOREIGN KEY (parentid)
      REFERENCES "UserManagement".groups (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "UserManagement".groups
  OWNER TO "UserManagement";


CREATE TABLE if not exists "UserManagement".grouppermission
(
  permissionid integer NOT NULL,
  groupid integer NOT NULL,
  state integer DEFAULT 0,
  CONSTRAINT grouppermission_pkey PRIMARY KEY (permissionid, groupid),
  CONSTRAINT grouppermission_groupid_fkey FOREIGN KEY (groupid)
      REFERENCES "UserManagement".groups (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT grouppermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES "UserManagement".permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "UserManagement".grouppermission
  OWNER TO "UserManagement";

CREATE TABLE if not exists "UserManagement".servicepermission
(
  id serial NOT NULL,
  service character varying(64),
  permissionid integer,
  CONSTRAINT servicepermission_pkey PRIMARY KEY (id),
  CONSTRAINT servicepermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES "UserManagement".permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "UserManagement".servicepermission
  OWNER TO "UserManagement";


CREATE TABLE if not exists "UserManagement".usergroup
(
  userid integer NOT NULL,
  groupid integer NOT NULL,
  CONSTRAINT usergroup_pkey PRIMARY KEY (userid, groupid),
  CONSTRAINT usergroup_groupid_fkey FOREIGN KEY (groupid)
      REFERENCES "UserManagement".groups (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT usergroup_userid_fkey FOREIGN KEY (userid)
      REFERENCES "UserManagement".users (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "UserManagement".usergroup
  OWNER TO "UserManagement";



CREATE TABLE if not exists "UserManagement".userpermission
(
  userid integer NOT NULL,
  permissionid integer NOT NULL,
  state integer DEFAULT 0,
  CONSTRAINT userpermission_pkey PRIMARY KEY (userid, permissionid),
  CONSTRAINT userpermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES "UserManagement".permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT userpermission_userid_fkey FOREIGN KEY (userid)
      REFERENCES "UserManagement".users (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "UserManagement".userpermission
  OWNER TO "UserManagement";


INSERT INTO permission(id,parentid,name,title) select 0,-1,'_masterPermission','Permissions' WHERE NOT EXISTS (SELECT id FROM permission WHERE id=0);

END;
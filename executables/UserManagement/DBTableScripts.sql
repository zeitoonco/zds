BEGIN;
CREATE TABLE if not exists  permission
(
  id serial NOT NULL,
  parentid integer,
  name character varying(64) NOT NULL,
  title character varying(64) DEFAULT NULL::character varying,
  description character varying(1024) DEFAULT NULL::character varying,
  CONSTRAINT permission_pkey PRIMARY KEY (id),
  CONSTRAINT permission_parentid_fkey FOREIGN KEY (parentid)
      REFERENCES  permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT permission_name_key UNIQUE (name)
);


CREATE TABLE if not exists  users
(
 id serial NOT NULL,
  username character varying(32),
  password character varying(255),
  name character varying(255),
  banned boolean DEFAULT false,
  banreason character varying(255) DEFAULT NULL::character varying,
  avatar text DEFAULT NULL,
  CONSTRAINT users_pkey PRIMARY KEY (id),
  CONSTRAINT users_username_key UNIQUE (username)
);


CREATE TABLE if not exists  groups
(
   id serial NOT NULL,
  title character varying(64),
  parentid integer,
  description character varying(4096) DEFAULT NULL::character varying,
  CONSTRAINT groups_pkey PRIMARY KEY (id),
  CONSTRAINT groups_parentid_fkey FOREIGN KEY (parentid)
      REFERENCES  groups (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT
);


CREATE TABLE if not exists  grouppermission
(
  permissionid integer NOT NULL,
  groupid integer NOT NULL,
  state integer DEFAULT 0,
  CONSTRAINT grouppermission_pkey PRIMARY KEY (permissionid, groupid),
  CONSTRAINT "grouppermission_GID" FOREIGN KEY (groupid)
      REFERENCES  groups (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT "grouppermission_PID" FOREIGN KEY (permissionid)
      REFERENCES  permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
);


CREATE TABLE if not exists  servicepermission
(
   id serial NOT NULL,
  service character varying(64),
  permissionid integer,
  CONSTRAINT servicepermission_pkey PRIMARY KEY (id),
  CONSTRAINT servicepermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES  permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
      );


CREATE TABLE if not exists  usergroup
(
  userid integer NOT NULL,
  groupid integer NOT NULL,
  CONSTRAINT usergroup_pkey PRIMARY KEY (userid, groupid),
  CONSTRAINT usergroup_groupid_fkey FOREIGN KEY (groupid)
      REFERENCES  groups (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT usergroup_userid_fkey FOREIGN KEY (userid)
      REFERENCES  users (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
);


CREATE TABLE if not exists  userpermission
(
 userid integer NOT NULL,
  permissionid integer NOT NULL,
  state integer DEFAULT 0,
  CONSTRAINT userpermission_pkey PRIMARY KEY (userid, permissionid),
  CONSTRAINT userpermission_permissionid_fkey FOREIGN KEY (permissionid)
      REFERENCES  permission (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT userpermission_userid_fkey FOREIGN KEY (userid)
      REFERENCES  users (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
);


CREATE TABLE  contacts
(
  userid integer NOT NULL,
  contactid integer NOT NULL,
  note character varying,
  CONSTRAINT contacts_pkey PRIMARY KEY (userid, contactid),
  CONSTRAINT contacts_contactid_fkey FOREIGN KEY (contactid)
      REFERENCES  users (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT contacts_userid_fkey FOREIGN KEY (userid)
      REFERENCES  users (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
);


--MASTER PERMISSION
INSERT INTO permission(id,parentid,name,title) select 0, NULL,'_masterPermission','Permissions' WHERE NOT EXISTS (SELECT id FROM permission WHERE id=0);



--TRIGGER FUNCTIONS
CREATE OR REPLACE FUNCTION  userpermissiongt()
  RETURNS trigger AS
$BODY$ BEGIN
	IF TG_OP='DELETE' THEN
		IF OLD.userid=0 THEN
			RAISE EXCEPTION USING MESSAGE='CANNOT REMOVE AN ADMIN PRIVILEGE';
		END IF;
		RETURN OLD;
	END IF;
	IF TG_OP='INSERT' THEN
		IF NEW.userid=0 THEN
		RAISE EXCEPTION USING MESSAGE='CANNOT CREATE AN ADMIN PRIVILEGE';
		END IF;
		RETURN NEW;
	END IF;
	IF TG_OP='UPDATE' THEN
		IF OLD.userid=0 THEN
			RAISE EXCEPTION USING MESSAGE='CANNOT MODIFY AN ADMIN PRIVILEGE';
		END IF;
		RETURN NEW;
	END IF;
END;$BODY$
  LANGUAGE plpgsql VOLATILE;
-------------
CREATE OR REPLACE FUNCTION  usersgt()
  RETURNS trigger AS
$BODY$

BEGIN
	IF TG_OP='DELETE' THEN
		IF (OLD.id=0) THEN
		RAISE EXCEPTION USING MESSAGE='CANNOT REMOVE ADMIN ACCOUNT';
		END IF;
		RETURN OLD;
	END IF;


	IF TG_OP='UPDATE' THEN
		IF (OLD.id =0 OR OLD.username='admin') THEN
			NEW.banned=FALSE;
			NEW.username=OLD.username;
		END IF;
		NEW.id= OLD.id;
		RETURN NEW;
	END IF;


RETURN NEW;
END;$BODY$
  LANGUAGE plpgsql VOLATILE;


---TRIGGERS
  CREATE TRIGGER userpermissiontrigger
  BEFORE INSERT OR UPDATE OR DELETE
  ON  userpermission
  FOR EACH ROW
  EXECUTE PROCEDURE  userpermissiongt();
  ---------------
  CREATE TRIGGER userstrigger
  BEFORE INSERT OR UPDATE OR DELETE
  ON  users
  FOR EACH ROW
  EXECUTE PROCEDURE  usersgt();
  ------------------------------------------------------
  ---FUNCTIONS
  CREATE OR REPLACE FUNCTION  addusersusergroup(
    IN uid integer,
    IN p1 integer[])
  RETURNS TABLE(userid integer, groupid integer, errormsg text, sqlstate text) AS
$BODY$
	DECLARE
		i	integer;
	BEGIN
--FOREACH i IN ARRAY p1
	      FOR i IN array_lower(p1, 1).. array_upper(p1, 1)
	       loop
	       BEGIN

		     insert into usergroup(userid, groupid) values(uid,P1[i]);
	       EXCEPTION

		 WHEN  OTHERS THEN
		  WHEN  OTHERS THEN
			RETURN QUERY SELECT uid, P1[i], SQLERRM, SQLSTATE ;
	       END;

	       end loop;
	END $BODY$
  LANGUAGE plpgsql VOLATILE;
  -----------------------
  CREATE OR REPLACE FUNCTION  removeusersusergroup(
    IN uid integer,
    IN p1 integer[])
  RETURNS TABLE(userid integer, groupid integer, errormsg text, sqlstate text) AS
$BODY$
	DECLARE
		i	integer;
	BEGIN
RAISE NOTICE 'REMOVING GROUPS FOR USER: %', uid;
	       FOR i IN 1 .. array_upper(P1, 1)
	       loop
	       BEGIN
RAISE NOTICE 'REMOVING %',P1[i];
		     DELETE FROM usergroup WHERE ("usergroup".userid =uid AND "usergroup".groupid=P1[i]);
	       EXCEPTION

		 WHEN  OTHERS THEN
		  WHEN  OTHERS THEN
			RETURN QUERY SELECT uid , P1[i], SQLERRM, SQLSTATE ;
	       END;

	       end loop;

	END $BODY$
  LANGUAGE plpgsql VOLATILE;
  --------------------
  CREATE OR REPLACE FUNCTION  setgrouppermission(
    IN gid integer,
    IN p1 integer[])
  RETURNS TABLE(pid integer, pstate integer, errormsg text, sqlstate text) AS
$BODY$
	DECLARE
		i	integer;
	BEGIN
	       FOR i IN 1 .. array_upper(P1, 1)
	       loop
	       BEGIN
		     insert into grouppermission values(P1[i][1],GID,P1[i][2])
		      ON CONFLICT (permissionid,groupid) DO UPDATE SET state=P1[i][2];
	       EXCEPTION

		 WHEN  OTHERS THEN
		  WHEN  OTHERS THEN
			RETURN QUERY SELECT P1[i][1] , P1[i][2], SQLERRM, SQLSTATE ;
	       END;

	       end loop;

	END $BODY$
  LANGUAGE plpgsql VOLATILE;
  -------------------------
  CREATE OR REPLACE FUNCTION  setuserpermission(
    IN uid integer,
    IN p1 integer[])
  RETURNS TABLE(pid integer, pstate integer, errormsg text, sqlstate text) AS
$BODY$
	DECLARE
		i integer;

	BEGIN

	       FOR i IN 1 .. array_upper(P1, 1)
	       loop
	       BEGIN

		     insert into userpermission values(uid,P1[i][1],P1[i][2])
		      ON CONFLICT (permissionid,userid) DO UPDATE SET state=P1[i][2];

	       EXCEPTION

		 WHEN  OTHERS THEN
			RETURN QUERY SELECT P1[i][1] , P1[i][2], SQLERRM, SQLSTATE ;
	       END;

	       end loop;


	END $BODY$
  LANGUAGE plpgsql VOLATILE;
END;

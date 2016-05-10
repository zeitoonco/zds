BEGIN;

CREATE TABLE if not exists UserData
(
  UserID integer NOT NULL,
  status integer NOT NULL,
  customStatusIcon integer,
  customStatusTex character varying(255),
  reachState integer,
  CONSTRAINT UserData_pkey PRIMARY KEY (UserID)
)
WITH (
  OIDS=FALSE
);
CREATE TABLE if not exists Session
(
  id serial NOT NULL,
  creationDate date NOT NULL,
  CONSTRAINT unique_id PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);


CREATE TABLE if not exists Message
(
  id serial NOT NULL,
  userID integer NOT NULL,
  sessionID integer NOT NULL,
  msg character varying(4096) NOT NULL,
  msgDate date NOT NULL,
  type integer,
  CONSTRAINT Message_pkey PRIMARY KEY (id),
  CONSTRAINT lnk_Message_Session FOREIGN KEY (sessionID)
      REFERENCES Session (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);
CREATE INDEX index_id
  ON Message
  USING btree
  (id);


CREATE TABLE if not exists SessionUser
(
  UserID integer NOT NULL,
  SessionID integer NOT NULL,
  joined timestamp with time zone NOT NULL,
  seenID bigserial,
  notifiedID bigserial,
  CONSTRAINT SessionUser_pkey PRIMARY KEY (UserID, SessionID),
  CONSTRAINT lnk_SessionUser_Message FOREIGN KEY (seenID)
      REFERENCES Message (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT lnk_SessionUser_Message_2 FOREIGN KEY (notifiedID)
      REFERENCES Message (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT lnk_SessionUser_Session FOREIGN KEY (SessionID)
      REFERENCES Session (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);

END;
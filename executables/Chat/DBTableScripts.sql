BEGIN;

CREATE TABLE if not exists userdata
(
  userid integer NOT NULL,
  status integer NOT NULL,
  customstatusicon integer,
  customstatustext character varying(255),
  reachstate integer,
  CONSTRAINT userdata_pkey PRIMARY KEY (userid)
)
WITH (
  OIDS=FALSE
);

CREATE TABLE if not exists session
(
  id serial NOT NULL,
  creationdate timestamp with time zone NOT NULL DEFAULT now(),
  Leader bigint,
  CONSTRAINT unique_id PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

CREATE TABLE if not exists Message
(
  id serial NOT NULL,
  userid integer NOT NULL,
  sessionid integer NOT NULL,
  msg character varying(4096) NOT NULL,
  msgdate timestamp with time zone NOT NULL DEFAULT now(),
  type integer,
  CONSTRAINT message_pkey PRIMARY KEY (id),
  CONSTRAINT lnk_message_session FOREIGN KEY (sessionid)
      REFERENCES "Chat".session (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);

CREATE TABLE if not exists SessionUser
(
  userid integer NOT NULL,
  sessionid integer NOT NULL,
  joined timestamp with time zone NOT NULL DEFAULT now(),
  seenid bigint,
  notifiedid bigint,
  CONSTRAINT sessionuser_pkey PRIMARY KEY (userid, sessionid),
  CONSTRAINT lnk_sessionuser_message FOREIGN KEY (seenid)
      REFERENCES "Chat".message (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT lnk_sessionuser_message_2 FOREIGN KEY (notifiedid)
      REFERENCES "Chat".message (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT lnk_sessionuser_session FOREIGN KEY (sessionid)
      REFERENCES "Chat".session (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);

END;
#!/usr/bin/python3
# Copyright (c) BDist Development Team
# Distributed under the terms of the Modified BSD License.
import os
from logging.config import dictConfig
import datetime
from flask import Flask, jsonify, request
from flask_limiter import Limiter
from flask_limiter.util import get_remote_address
from psycopg import DatabaseError, IsolationLevel
from psycopg.rows import namedtuple_row
from psycopg_pool import ConnectionPool

dictConfig(
    {
        "version": 1,
        "formatters": {
            "default": {
                "format": "[%(asctime)s] %(levelname)s in %(module)s:%(lineno)s - %(funcName)20s(): %(message)s",
            }
        },
        "handlers": {
            "wsgi": {
                "class": "logging.StreamHandler",
                "stream": "ext://flask.logging.wsgi_errors_stream",
                "formatter": "default",
            }
        },
        "root": {"level": "INFO", "handlers": ["wsgi"]},
    }
)

RATELIMIT_STORAGE_URI = os.environ.get("RATELIMIT_STORAGE_URI", "memory://")

app = Flask(__name__)
app.config.from_prefixed_env()
log = app.logger
limiter = Limiter(
    get_remote_address,
    app=app,
    default_limits=["200 per day", "50 per hour"],
    storage_uri=RATELIMIT_STORAGE_URI,
)

# Use the DATABASE_URL environment variable if it exists, otherwise use the default.
# Use the format postgres://username:password@hostname/database_name to connect to the database.
DATABASE_URL = os.environ.get("DATABASE_URL", "postgres://aviacao:aviacao@postgres/aviacao")

pool = ConnectionPool(
    conninfo=DATABASE_URL,
    kwargs={
        "autocommit": True,  # If True don’t start transactions automatically.
        "row_factory": namedtuple_row,
    },
    min_size=4,
    max_size=10,
    open=True,
    # check=ConnectionPool.check_connection,
    name="postgres_pool",
    timeout=5,
)


def is_decimal(s):
    """Returns True if string is a parseable float number."""
    try:
        float(s)
        return True
    except ValueError:
        return False


@app.route("/", methods=("GET",))
@limiter.limit("1 per second")
def airport_view():
    """Show all the airports"""

    with pool.connection() as conn:
        with conn.cursor() as cur:
            aeroportos = cur.execute(
                """
                SELECT nome, cidade 
                FROM aeroporto;
                """,
                {},
            ).fetchall()
            log.debug(f"Found {cur.rowcount} rows.")

    return jsonify(aeroportos), 200


@app.route("/voos/<partida>", methods=("GET",))
@limiter.limit("1 per second")
def next_departure_flight(partida):
    """Show the page to update the account balance."""
    now=datetime.datetime.now()
 
    with pool.connection() as conn:
        with conn.cursor() as cur:
            aeroporto = cur.execute(
                """
                select * from aeroporto where codigo=%s;
                """,
                (partida,),
            ).fetchall()
            if len(aeroporto)==0:
                return "Airport not found", 404
            prepareddate=str(now.year)+"-"+str(now.month)+"-"+str(now.day)+" "+str(now.hour)+":"+str(now.minute)+":"+str(now.second)
            now+=datetime.timedelta(hours=12)
            postprepare=str(now.year)+"-"+str(now.month)+"-"+str(now.day)+" "+str(now.hour)+":"+str(now.minute)+":"+str(now.second)
            voos = cur.execute(
                """
                SELECT no_serie, hora_partida, chegada FROM  voo where voo.partida=%s AND voo.hora_partida> %s AND voo.hora_partida<=  %s
                """,
                (partida,prepareddate,postprepare),
            ).fetchall()
            log.debug(f"Found {cur.rowcount} rows.")

    # At the end of the `connection()` context, the transaction is committed
    # or rolled back, and the connection returned to the pool.


    return jsonify(voos), 200


@app.route(
    "/voos/<partida>/<chegada>/",
    methods=("GET",
    ),
)
def next_three(partida,chegada):
    """Next three flights between source and destination with free tickets"""
    now=datetime.datetime.now()
    with pool.connection() as conn:
        with conn.cursor() as cur:
            aeroporto = cur.execute(
                """
                select * from aeroporto where codigo=%s;
                """,
                (partida,),
            ).fetchall()
            if len(aeroporto)==0:
                return "Partida airport not found", 404
            aeroporto = cur.execute(
                """
                select * from aeroporto where codigo=%s;
                """,
                (chegada,),
            ).fetchall()
            if len(aeroporto)==0:
                return "Chegada airport not found", 404
            prepareddate=str(now.year)+"-"+str(now.month)+"-"+str(now.day)+" "+str(now.hour)+":"+str(now.minute)+":"+str(now.second)
            voos = cur.execute(
                """
                SELECT voos(%s, %s, %s);
                """,
                (partida,chegada,prepareddate),
            ).fetchmany(3)
            log.debug(f"Found {cur.rowcount} rows.")

    # At the end of the `connection()` context, the transaction is committed
    # or rolled back, and the connection returned to the pool.


    return jsonify(voos), 200


@app.route(
    "/compra/<voo>/",
    methods=(
        "POST",
    ),
)
# Needs to make the insert of the venda first, and then the tickets
def compra(voo):
    compra_date = datetime.datetime.now()
    nomes=request.args.getlist("nome")
    classes=request.args.getlist("classe")
    NIF= request.args.get("NIF")
    if not NIF:
        return "failed nif", 400  
    if len(classes)!=len(nomes):
        return "Número de classes diferente de número de passageiros", 400 
    
    for i in range(len(classes)):
        if classes[i]!="1c" and classes[i]!="2c":
           return "Por favor formatar a classes[i] como sendo 1c para primeira classes[i] e 2c para segunda classes[i]", 400
        if classes[i] =="1c":
           classes[i]=True
        if classes[i] =="2c":
            classes[i]=False
        
    try:
        with pool.connection() as conn:
                with conn.cursor() as cur:
                    aeroporto = cur.execute(
                            """
                            select * from voo where id=%s;
                            """,
                            (voo,),
                        ).fetchall()
                    if len(aeroporto)==0:
                        return "Flight not found", 404
                    ids=cur.execute(
                        """
                        SELECT compra( %s, %s, %s, %s,%s);
                        """,
                        (voo, NIF,nomes,classes, compra_date)
                    ).fetchall()
    except DatabaseError as error:
        error_msg = str(error).split('\n')[0]       
        return jsonify({"message": error_msg, "status": "error"}), 400


    return jsonify({"message": "Compra realizada com sucesso! O ID dos bilhetes é:"+str(ids[0][0]), "status": "success"}), 200
    #return jsonify([]), 200


# Atribuir um assento ao bilhete (check in) é definir o lugar e o no serie do bilhete, que previamente estavam NULL, pois estas sao as chaves dum assento
@app.route("/checkin/<bilhete>/", methods=("GET",))
@limiter.limit("1 per second")
def check_in(bilhete):

    # Is a datetime.datetime object - comparable to TIMESTAMP in SQL
    check_in_date = datetime.datetime.now()

    try:
        with pool.connection() as conn:
            with conn.cursor() as cur:
                aeroporto = cur.execute(
                """
                select * from bilhete where id=%s;
                """,
                (bilhete,),
                ).fetchall()
                if len(aeroporto)==0:
                    return "Ticket not found", 404
                cur.execute(
                    """

                    SELECT check_in( %s, %s);
                    
                    """,
                    (bilhete, check_in_date)
                )
                checked_in = cur.fetchall()
                log.debug("Bilhete %s checked-in!", bilhete)
                
    except DatabaseError as error:
        # error has more than just the message we specified in the .sql, we need to trim it
        error_msg = str(error).split('\n')[0]       
        # 400 = Bad Request pois o id dado não existe ou fez-se o comando para um bilhete dum voo que já partiu
        return jsonify({"message": error_msg, "status": "error"}), 400

    
    return jsonify({"message": "Bilhete foi checked-in com sucesso!, o seu lugar é:"+str(checked_in[0][0]), "status": "success"}), 200
#    return jsonify(checked_in), 200



@app.route("/ping", methods=("GET",))
@limiter.exempt
def ping():
    log.debug("ping!")
    return jsonify({"message": "pong!", "status": "success"})


if __name__ == "__main__":
    app.run()

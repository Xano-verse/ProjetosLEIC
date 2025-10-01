\copy aeroporto FROM ./aeroporto.txt
\copy aviao FROM ./aviao.txt
\copy assento FROM ./assento.txt
\copy voo FROM ./voo.txt
SELECT setval(
    pg_get_serial_sequence('voo', 'id'),
    COALESCE(MAX(id), 1),
    true
) FROM voo;
\copy venda FROM ./venda.txt
SELECT setval(
    pg_get_serial_sequence('venda', 'codigo_reserva'),
    COALESCE(MAX(codigo_reserva), 1),
    true
) FROM venda;
\copy bilhete FROM ./bilhete.txt  WITH (FORMAT text, DELIMITER E'\t', NULL 'NULL');
SELECT setval(
    pg_get_serial_sequence('bilhete', 'id'),
    COALESCE(MAX(id), 1),
    true
) FROM bilhete;

------------------------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION voos(
	r_partida CHAR(3),                               	
	r_chegada CHAR(3),
	hora_atual TIMESTAMP 
) RETURNS TABLE(
	r_no_serie VARCHAR(80),
	r_hora_partida TIMESTAMP
) AS
$$
BEGIN
	RETURN QUERY
	SELECT no_serie , hora_partida FROM voo
	WHERE partida = r_partida AND chegada = r_chegada AND hora_partida > hora_atual AND ((
		SELECT COUNT(*)
		FROM assento
		WHERE prim_classe = TRUE
		AND voo.no_serie = assento.no_serie
	) > (
		SELECT COUNT(*)
		FROM bilhete
		WHERE voo_id = id
		AND prim_classe = TRUE
	)
	OR
	(
		SELECT COUNT(*)
		FROM assento
		WHERE prim_classe = FALSE
		AND voo.no_serie = assento.no_serie
	) > (
		SELECT COUNT(*)
		FROM bilhete
		WHERE voo_id = id
		AND prim_classe = FALSE
	))
	ORDER BY hora_partida
	LIMIT 3;

END;
$$ LANGUAGE plpgsql;


------------------------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION compra(
	in_voo_id INTEGER,
	in_nif_cliente CHAR(9),
	in_nomes VARCHAR(80)[],
	in_classes BOOLEAN[],
	in_hora_compra TIMESTAMP
) RETURNS INTEGER[] AS
$$
DECLARE
	codigo_reserva_venda INTEGER;
	hora_partida_voo TIMESTAMP;
	hora_chegada_voo TIMESTAMP;
	no_serie_voo VARCHAR(80);
	i INT;
	tempID INTEGER;
	ticket_ids INTEGER[];
	preco_venda NUMERIC(7,2);
	preco_to_add NUMERIC(7,2);
	preco_true NUMERIC(7,2);
	
BEGIN
	-- Assumir sempre que balcão é NULL
	INSERT INTO venda (nif_cliente, balcao, hora) VALUES (in_nif_cliente, NULL, in_hora_compra) RETURNING codigo_reserva INTO codigo_reserva_venda;

	-- SELECT no_serie INTO no_serie_voo FROM voo WHERE id = in_voo_id;
	SELECT hora_partida, hora_chegada, no_serie INTO hora_partida_voo, hora_chegada_voo, no_serie_voo FROM voo WHERE id = in_voo_id;
	preco_venda := (EXTRACT(EPOCH FROM (hora_chegada_voo - hora_partida_voo)) / 3600 * 0.2)*50;
	preco_to_add := 0.5 * preco_venda;

	-- Locking down all the rows on the tickets table for a given flight
	
	PERFORM * FROM assento where no_serie= no_serie_voo FOR UPDATE;
	
	FOR i IN 1..array_length(in_classes, 1) 
	LOOP
		IF in_classes[i] = TRUE THEN 
			preco_true := preco_venda + preco_to_add;
		ELSE
			preco_true := preco_venda;
		END IF;

		-- lugar e no_serie do bilhete comecam NULL pois bilhete nao foi checked in
		INSERT INTO bilhete (voo_id, codigo_reserva, nome_passageiro, preco, prim_classe, lugar, no_serie) 
		VALUES (in_voo_id, codigo_reserva_venda, in_nomes[i], preco_true, in_classes[i], NULL, NULL) RETURNING cast(id as INTEGER) into tempID;
		select array_cat(ticket_ids,ARRAY[tempID] ) into ticket_ids;
	END LOOP;

	RETURN ticket_ids;
END;
$$ LANGUAGE plpgsql;


------------------------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION check_in(
	id_bilhete INTEGER,
	check_in_date TIMESTAMP
) RETURNS VARCHAR AS
$$
DECLARE
	classe_bilhete BOOLEAN;
	lugar_bilhete VARCHAR;
	no_serie_bilhete VARCHAR(80);
	voo_id_bilhete INTEGER;
	
	no_serie_voo VARCHAR(80);
	hora_partida_voo TIMESTAMP;

	r RECORD;			-- variavel do for loop
BEGIN
	-- Verificar por erros - se bcilhete existe e se o voo já partiu
	SELECT prim_classe, lugar, no_serie, voo_id INTO classe_bilhete ,lugar_bilhete, no_serie_bilhete, voo_id_bilhete FROM bilhete WHERE id = id_bilhete;

	IF lugar_bilhete IS NOT NULL OR no_serie_bilhete IS NOT NULL THEN
		RAISE EXCEPTION 'Bilhete já foi checked-in.';
	END IF;

	SELECT hora_partida INTO hora_partida_voo FROM voo WHERE id = voo_id_bilhete;

	IF check_in_date > hora_partida_voo THEN
		RAISE EXCEPTION 'Impossível fazer check-in: avião já descolou.';
	END IF;

	
	-- Vamos buscar o no serie associado ao voo do bilhete
	SELECT no_serie INTO no_serie_voo FROM voo WHERE id = voo_id_bilhete;
	PERFORM * FROM assento where no_serie= no_serie_voo FOR UPDATE;





	-- Percorrer os assentos todos criando uma tabela que corresponde cada assento do voo a cada bilhete do voo
	-- Quanto encontrarmos o bilhete com o id dado como input ficamos com esse lugar - é uma escolha de lugares à base de quem comprou os bilhetes primeiro basically, porque estamos a associar o 1o assento do voo ao 1o bilhete do voo and so on
	FOR r IN
		SELECT a.lugar AS lugar_a, b.lugar AS lugar_b, a.prim_classe AS classe_a
		-- keep this AND in the JOIN to keep it as a LEFT JOIN - vai manter os lugares nos assentos mesmo que os lugares nos bilhetes sejam NULL
		-- fazer join so dos bilhetes do voo que quero e depois selecionar so os assentos do aviao que quero
		FROM assento a LEFT JOIN bilhete b ON a.lugar = b.lugar 
		AND a.no_serie = b.no_serie AND b.voo_id = voo_id_bilhete 
		WHERE a.no_serie = no_serie_voo
	LOOP
		-- vejo o primeiro bilhete que tem o lugar a NULL e sei que o assento correspondente está livre 
		IF r.lugar_b IS NULL AND r.classe_a = classe_bilhete THEN
			lugar_bilhete := r.lugar_a;
			EXIT;
		END IF;
	END LOOP;


	-- Definir o no serie no bilhete para o no serie do voo dele e definir o lugar do bilhete
	-- Fazer os dois no mesmo UPDATE por causa do TRIGGER
	UPDATE bilhete SET no_serie = no_serie_voo, lugar = lugar_bilhete WHERE id = id_bilhete;

	RETURN lugar_bilhete;
END;
$$ LANGUAGE plpgsql;



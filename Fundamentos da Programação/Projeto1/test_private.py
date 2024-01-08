import pytest 
import Projeto1 as fp # <--- Change the name projectoFP to the file name with your project


class TestEhTerritorio:

    def test_1(self):
        grid = 1
        assert not fp.eh_territorio(grid)
        
    def test_2(self):
        grid = [(0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0)]
        assert not fp.eh_territorio(grid)
    
    def test_3(self):
        grid = ()
        assert not fp.eh_territorio(grid)
        
    def test_4(self):
        grid = ((1,0,0,1),)*27
        assert not fp.eh_territorio(grid)
        
    def test_5(self):
        grid = (1,)
        assert not fp.eh_territorio(grid)
        
    def test_6(self):
        grid = ((0,1,0,0),1,(0,0,1,0),(1,0,0,0),(0,0,0,0))
        assert not fp.eh_territorio(grid)
    
    def test_7(self):
        grid = ((0,1,0,0),[0,0,0,0])
        assert not fp.eh_territorio(grid)
    
    def test_8(self):
        grid = ((0,1,0,0)*50,(0,0,1,0)*50)
        assert not fp.eh_territorio(grid)
        
    def test_9(self):
        grid = ((0,1,0,0),(0,0,0,0),(1,0,0,0),(0,0,0,0,1))
        assert not fp.eh_territorio(grid)
        
    def test_10(self):
        grid = ((0,1,0,0),(0,0,1,0),(1,0,0,0),(2,0,0,0))
        assert not fp.eh_territorio(grid)
    
    def test_11(self):
        grid = ((0,1,0,0),(0,0,1,0),(1,0,0,0),(1.0,0,0,0))
        assert not fp.eh_territorio(grid)
        
    def test_12(self):
        grid =  ((1,)*100,)*26
        assert not fp.eh_territorio(grid)
        
    def test_13(self):
        grid =  ((1,)*99,)*26
        assert fp.eh_territorio(grid)
        
    def test_14(self):
        grid =  ((1,),)
        assert fp.eh_territorio(grid)
        

class TestEhIntersecao:

    def test_1(self):
        pos = 1
        assert not fp.eh_intersecao(pos)
        
    def test_2(self):
        pos = ()
        assert not fp.eh_intersecao(pos)    
    
    def test_3(self):
        pos = ['A', 2]
        assert not fp.eh_intersecao(pos)    
     
    def test_4(self):
        pos = ('A', 2, 5)
        assert not fp.eh_intersecao(pos)    
        
    def test_5(self):
        pos = ('A', 2, 5)
        assert not fp.eh_intersecao(pos)  
    
    def test_6(self):
        pos = (2, 'A')
        assert not fp.eh_intersecao(pos)  
        
    def test_7(self):
        pos = ('a', 2)
        assert not fp.eh_intersecao(pos)  
        
    def test_8(self):
        pos = ('A', 2.5)
        assert not fp.eh_intersecao(pos)  
        
    def test_9(self):
        pos = ('AB', 2)
        assert not fp.eh_intersecao(pos)  
        
    def test_10(self):
        pos = ('B', -2)
        assert not fp.eh_intersecao(pos)    
        
    def test_11(self):
        pos = ('B', 112)
        assert not fp.eh_intersecao(pos)    
    
    def test_12(self):
        pos = ('', 2)
        assert not fp.eh_intersecao(pos)  
        
    def test_13(self):
        pos = ('B',)
        assert not fp.eh_intersecao(pos)  
        
    def test_14(self):
        pos = ('A', 1)
        assert fp.eh_intersecao(pos)  
       
    def test_15(self):
        pos = ('Z', 99)
        assert fp.eh_intersecao(pos)  
         
class TestEhIntersecaoValida:
    
    def test_1(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0))
        assert fp.eh_intersecao_valida(grid, ('E', 4))
    
    def test_2(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0))
        assert not fp.eh_intersecao_valida(grid, ('D', 5))
        
    def test_3(self):    
        grid =  ((1,)*99,)*26
        assert  all([fp.eh_intersecao_valida(grid, (c,r)) for c in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' for r in range(1,50,4)])
      
        
class TestEhIntersecaoLivre:
    def test_1(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(1,0,0,0))
        assert fp.eh_intersecao_livre(grid, ('E', 4))     
     
    def test_2(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0))
        assert not fp.eh_intersecao_livre(grid, ('D', 1))    
        
    def test_3(self):    
        grid = ((1,1,1,1),(1,1,1,1),(1,1,1,1))
        assert all([not fp.eh_intersecao_livre(grid, (c, l)) for c in 'ABC' for l in (1,2,3,4)])
        
    def test_4(self):    
        grid = ((0,0,0,0),(0,0,0,0),(0,0,0,0))
        assert all([fp.eh_intersecao_livre(grid, (c, l)) for c in 'ABC' for l in (1,2,3,4)])
        

class TestObtemIntersecoesAdjacentes:
    def test_1(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0))
        assert (('E',3), ('D',4)) == fp.obtem_intersecoes_adjacentes(grid, ('E', 4))     
     
    def test_2(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0), (0,0,0,0))
        assert (('E', 2), ('D', 3), ('F', 3), ('E', 4)) == fp.obtem_intersecoes_adjacentes(grid, ('E', 3))     
     
    def test_3(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0), (0,0,0,0))
        assert (('A', 2), ('B', 3), ('A', 4)) == fp.obtem_intersecoes_adjacentes(grid, ('A', 3))     
     
    def test_4(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0), (0,0,0,0))
        assert (('D', 3), ('C', 4), ('E', 4)) == fp.obtem_intersecoes_adjacentes(grid, ('D', 4))     
     
    def test_5(self):    
        grid = ((0,),)
        assert () == fp.obtem_intersecoes_adjacentes(grid, ('A', 1))     
     
    def test_6(self):    
        grid =  ((1,)*99,)*26
        assert (('Z', 98), ('Y', 99)) == fp.obtem_intersecoes_adjacentes(grid, ('Z', 99)) 
        

class TestObtemUltimaIntersecao:
    def test_1(self):    
        grid = ((0,1,0,0),(0,0,0,0),(0,0,1,0),(1,0,0,0),(0,0,0,0),(0,0,0,0))
        assert ('F', 4) == fp.obtem_ultima_intersecao(grid)     
     
    def test_2(self):    
        grid = ((0,),)
        assert ('A', 1) == fp.obtem_ultima_intersecao(grid)    
        
    def test_3(self):    
        grid =  ((1,)*99,)*26
        assert ('Z', 99) == fp.obtem_ultima_intersecao(grid)    
       
       
class TestOrdenaIntersecoes:
    def test_1(self):
        tup = ('C', 4), ('E', 5), ('D', 4), ('B', 1), ('F', 4), ('D', 3), ('B', 2), ('C', 3), ('E', 2), ('E', 1) 
        ref = ('B', 1), ('E', 1), ('B', 2), ('E', 2), ('C', 3), ('D', 3), ('C', 4), ('D', 4), ('F', 4), ('E', 5)
        assert ref == fp.ordena_intersecoes(tup)    
        
    def test_2(self):
        tup = () 
        ref = ()
        assert ref == fp.ordena_intersecoes(tup)  
        
    def test_3(self):
        tup = ('C', 1), ('C', 11), ('C', 4), ('B', 7), ('A', 2), ('A', 5), ('C', 7), ('B', 13), ('B', 14), ('C', 8)
        ref = ('C', 1), ('A', 2), ('C', 4), ('A', 5), ('B', 7), ('C', 7), ('C', 8), ('C', 11), ('B', 13), ('B', 14)
        assert ref == fp.ordena_intersecoes(tup)  
    
    def test_4(self):    
        tup = ('C', 1), ('D', 1), ('C', 2), ('D', 2), ('H', 1), ('F', 2), ('B', 1), ('H', 2), ('E', 2), ('F', 1)
        ref = ('B', 1), ('C', 1), ('D', 1), ('F', 1), ('H', 1), ('C', 2), ('D', 2), ('E', 2), ('F', 2), ('H', 2)
        assert ref == fp.ordena_intersecoes(tup)  
        
class TestTerritorioParaStr:
    def test_1(self):
        grid = ((0,1,0,0),(0,0,0,1),(0,1,1,0),(1,0,0,0),(0,0,0,0), (0,0,0,0),(1,0,1,0), (0,1,1,0))
        ref = '   A B C D E F G H\n 4 . X . . . . . .  4\n 3 . . X . . . X X  3\n 2 X . X . . . . X  2\n 1 . . . X . . X .  1\n   A B C D E F G H'
        assert ref == fp.territorio_para_str(grid)
        
    def test_2(self):
        grid = ((0,1,0,0,0,0,0,0,0,0,1,0),(1,0,0,0,0,1,1,1,1,0,0,0))
        ref = '   A B\n12 . . 12\n11 X . 11\n10 . . 10\n 9 . X  9\n 8 . X  8\n 7 . X  7\n 6 . X  6\n 5 . .  5\n 4 . .  4\n 3 . .  3\n 2 X .  2\n 1 . X  1\n   A B'
        assert ref == fp.territorio_para_str(grid)
        
    def test_3(self):
        grid = ((0,),)
        ref = '   A\n 1 .  1\n   A' 
        assert ref == fp.territorio_para_str(grid)

    def test_4(self):
        grid = ((0,0),)*9 + ((0,1),)*9 + ((1,1),)*8
        ref = '   A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n 2 . . . . . . . . . X X X X X X X X X X X X X X X X X  2\n 1 . . . . . . . . . . . . . . . . . . X X X X X X X X  1\n   A B C D E F G H I J K L M N O P Q R S T U V W X Y Z'
        assert ref == fp.territorio_para_str(grid)

    def test_5(self):
        grid = ((1, 1, 1, 1), (1, 1, 0, 1), (1, 0, 1, 0), (0, 1, 1, 0),
                (0, 0, 1, 0), (0, 0, 0, 0), (0, 0, 0, 0), (1, 1, 1, 0),
                (0, 0, 1, 1), (0, 1, 0, 1), (0, 1, 0, 0), (0, 1, 0, 0),
                (0, 1, 1, 0), (1, 0, 1, 1), (0, 0, 1, 1), (0, 1, 1, 1),
                (1, 0, 0, 0), (0, 0, 0, 0), (1, 1, 0, 0), (1, 0, 1, 1))
        
        ref = '   A B C D E F G H I J K L M N O P Q R S T\n 4 X X . . . . . . X X . . . X X X . . . X  4\n 3 X . X X X . . X X . . . X X X X . . . X  3\n 2 X X . X . . . X . X X X X . . X . . X .  2\n 1 X X X . . . . X . . . . . X . . X . X X  1\n   A B C D E F G H I J K L M N O P Q R S T'
        assert ref == fp.territorio_para_str(grid)

    def test_6(self):
        grid = ((0, 0, 0, 1, 0, 1, 0, 1),
                (0, 0, 0, 0, 1, 0, 0, 1),
                (1, 1, 1, 0, 1, 0, 0, 1),
                (0, 1, 1, 0, 1, 1, 0, 1),
                (1, 0, 0, 0, 1, 0, 1, 0),
                (0, 0, 0, 1, 1, 1, 0, 1),
                (0, 0, 1, 1, 1, 0, 0, 1),
                (1, 1, 1, 0, 0, 1, 1, 0))
        ref = '   A B C D E F G H\n 8 X X X X . X X .  8\n 7 . . . . X . . X  7\n 6 X . . X . X . X  6\n 5 . X X X X X X .  5\n 4 X . . . . X X .  4\n 3 . . X X . . X X  3\n 2 . . X X . . . X  2\n 1 . . X . X . . X  1\n   A B C D E F G H'
        assert ref == fp.territorio_para_str(grid)

class TestObtemCadeia:
    def test_1(self):
        grid = ((0,0,0),(0,1,0),(1,0,1),(0,1,0),(0,0,0))
        pos = 'C', 2 
        ref = (('C', 2),)
        assert ref == fp.obtem_cadeia(grid, pos)
         
    def test_2(self):
        grid = ((0,0,0),(0,1,0),(1,0,1),(0,1,0),(0,0,0))
        pos = 'B', 2 
        ref = (('B', 2),)
        assert ref == fp.obtem_cadeia(grid, pos)

    def test_3(self):
        grid = ((0,0,0),(0,1,0),(1,0,1),(0,1,0),(0,0,0))
        pos = 'A', 1 
        ref = (('A', 1), ('B', 1), ('A', 2), ('A', 3), ('B', 3))
        assert ref == fp.obtem_cadeia(grid, pos)

    def test_4(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,1,0),(0,0,0))
        pos = 'C', 2 
        ref = (('A', 1), ('E', 1), ('A', 2), ('C', 2), ('E', 2), ('A', 3), ('B', 3), ('C', 3), ('D', 3), ('E', 3))
        assert ref == fp.obtem_cadeia(grid, pos)
        
    def test_6(self):
        grid = ((0, 0, 0, 1, 0, 1, 0, 1),
                (0, 0, 0, 0, 1, 0, 0, 1),
                (1, 1, 1, 0, 1, 0, 0, 1),
                (0, 1, 1, 0, 1, 1, 0, 1),
                (1, 0, 0, 0, 1, 0, 1, 0),
                (0, 0, 0, 1, 1, 1, 0, 1),
                (0, 0, 1, 1, 1, 0, 0, 1),
                (1, 1, 1, 0, 0, 1, 1, 0))
        pos = 'B', 5 
        ref = (('H', 1), ('H', 2), ('G', 3), ('H', 3), ('F', 4), ('G', 4), ('B', 5),
               ('C', 5), ('D', 5), ('E', 5), ('F', 5), ('G', 5), ('D', 6), ('F', 6))
        assert ref == fp.obtem_cadeia(grid, pos)
        
    def test_7(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,1,0),(0,0,0))
        pos = 'C2'
        with pytest.raises(ValueError) as excinfo:
            fp.obtem_cadeia(grid, pos)
        assert "obtem_cadeia: argumentos invalidos" == str(excinfo.value)

    def test_8(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,0,0),(0,0,0))
        pos = 'F', 2
        with pytest.raises(ValueError) as excinfo:
            fp.obtem_cadeia(grid, pos)
        assert "obtem_cadeia: argumentos invalidos" == str(excinfo.value)


        
class TestObtemVale:
    def test_1(self):
        grid = ((0,0,0),(0,1,0),(1,0,1),(0,1,0),(0,0,0))
        pos = 'B', 2 
        ref = (('B', 1), ('A', 2), ('C', 2), ('B', 3))
        assert ref == fp.obtem_vale(grid, pos)
         
    def test_2(self):
        grid = ((0,0,0),(0,1,0),(1,0,1),(1,1,0),(0,0,0))
        pos = 'D', 1 
        ref = (('B', 1), ('E', 1), ('C', 2), ('E', 2), ('D', 3))
        assert ref == fp.obtem_vale(grid, pos)

    def test_3(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,0))
        pos = 'C', 2 
        ref = (('B', 1), ('D', 1), ('A', 2), ('E', 2), ('B', 3), ('D', 3))
        assert ref == fp.obtem_vale(grid, pos)

    def test_4(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,1,0),(0,0,0))
        pos = 'C', 1 
        ref = (('A', 1), ('E', 1), ('A', 2), ('C', 2), ('E', 2), ('B', 3), ('D', 3))
        assert ref == fp.obtem_vale(grid, pos)
        
    def test_5(self):
        grid = ((1,1,1),(1,1,1),(1,1,1),(1,1,1),(1,1,1))
        pos = 'C', 1 
        ref = ()
        assert ref == fp.obtem_vale(grid, pos)
        
    def test_6(self):
        grid = ((1,1,1),(1,0,1),(1,1,1),(1,1,1),(1,1,1))
        pos = 'C', 1 
        ref = (('B', 2),)
        assert ref == fp.obtem_vale(grid, pos)
      
    def test_7(self):
        grid = ((0, 0, 0, 1, 0, 1, 0, 1),
                (0, 0, 0, 0, 1, 0, 0, 1),
                (1, 1, 1, 0, 1, 0, 0, 1),
                (0, 1, 1, 0, 1, 1, 0, 1),
                (1, 0, 0, 0, 1, 0, 1, 0),
                (0, 0, 0, 1, 1, 1, 0, 1),
                (0, 0, 1, 1, 1, 0, 0, 1),
                (1, 1, 1, 0, 0, 1, 1, 0))
        pos = 'B', 5 
        ref = (('G', 1), ('G', 2), ('F', 3), ('B', 4), ('C', 4), ('D', 4), ('E', 4), ('H', 4),
               ('A', 5), ('H', 5), ('B', 6), ('C', 6), ('E', 6), ('G', 6), ('D', 7), ('F', 7))
        assert ref == fp.obtem_vale(grid, pos)
     
       
    def test_8(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,1,0,1),(0,0,0))
        pos = 'C', 1
        with pytest.raises(ValueError) as excinfo:
            fp.obtem_vale(grid, pos)
        assert "obtem_vale: argumentos invalidos" == str(excinfo.value)

    def test_9(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,1,0),(0,0,0))
        pos = 'C', 5
        with pytest.raises(ValueError) as excinfo:
            fp.obtem_vale(grid, pos)
        assert "obtem_vale: argumentos invalidos" == str(excinfo.value)
        
    def test_10(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,0,0),(0,0,0))
        pos = 'E', 2
        with pytest.raises(ValueError) as excinfo:
            fp.obtem_vale(grid, pos)
        assert "obtem_vale: argumentos invalidos" == str(excinfo.value)
 

class TestVerificaConexao:
    def test_1(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,0))
        assert fp.verifica_conexao(grid, ('A',1), ('B', 3))

    def test_2(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,0))
        assert fp.verifica_conexao(grid, ('C',1), ('C', 3))

    def test_3(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,0))
        assert not fp.verifica_conexao(grid, ('A',2), ('C', 2))

    def test_4(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,0))
        assert not fp.verifica_conexao(grid, ('A',3), ('E', 1))

    def test_5(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,1))
        assert not fp.verifica_conexao(grid, ('D',2), ('E', 3))

    def test_6(self):
        grid = ((0,0,0),(1,1,0),(1,0,0),(1,1,0),(0,0,0))
        assert fp.verifica_conexao(grid, ('A',1), ('E', 3))

    
    def test_7(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,1))
        with pytest.raises(ValueError) as excinfo:
            fp.verifica_conexao(grid, ('D',2), ('e', 3))
        assert "verifica_conexao: argumentos invalidos" == str(excinfo.value)
 
    def test_8(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,1))
        with pytest.raises(ValueError) as excinfo:
            fp.verifica_conexao(grid, ('D',2), ('F', 3))
        assert "verifica_conexao: argumentos invalidos" == str(excinfo.value)
 

class TestCalculaNumeroMontanhas:
    def test_1(self):
        grid =  ((1,)*29,)*12
        assert fp.calcula_numero_montanhas(grid) == 348 

    def test_2(self):
        grid =  ((0,)*29,)*12
        assert fp.calcula_numero_montanhas(grid) == 0 

    def test_3(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,1))
        assert fp.calcula_numero_montanhas(grid) == 6 

    def test_4(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,2,0),(0,0,1))
        with pytest.raises(ValueError) as excinfo:
            fp.calcula_numero_montanhas(grid)
        assert "calcula_numero_montanhas: argumento invalido" == str(excinfo.value)
 
 
class TestCalculaNumeroCadeiasMontanhas:
    def test_1(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,1))
        assert fp.calcula_numero_cadeias_montanhas(grid) == 2
 
    def test_2(self):
        grid =  ((1,)*29,)*12
        assert fp.calcula_numero_cadeias_montanhas(grid) == 1
        
    def test_3(self):
        grid =  ((0,)*29,)*12
        assert fp.calcula_numero_cadeias_montanhas(grid) == 0

    def test_4(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,0))
        assert fp.calcula_numero_cadeias_montanhas(grid) == 1
         
    def test_5(self):
        grid = ((0,0,0),(0,1,1),(1,1,1),(1,1,0),(0,0,0), (0,0,0),(0,1,1),(1,0,1),(1,1,0),(0,0,0),(0,0,0),(0,1,1),(1,0,1),(1,1,0),(1,1,0))
        assert fp.calcula_numero_cadeias_montanhas(grid) == 5
        
    def test_6(self):
        grid = ((1, 1, 1, 0, 1, 0, 1, 0, 0, 0),
                (1, 1, 0, 0, 1, 0, 1, 1, 0, 0),
                (1, 1, 1, 0, 0, 0, 0, 0, 0, 1),
                (0, 1, 0, 0, 1, 1, 1, 1, 0, 1),
                (0, 0, 0, 1, 0, 0, 1, 1, 0, 1),
                (0, 1, 0, 1, 1, 0, 0, 1, 1, 1),
                (1, 1, 1, 1, 1, 1, 1, 1, 1, 1),
                (0, 0, 0, 0, 1, 1, 0, 1, 1, 0),
                (0, 1, 0, 1, 1, 0, 1, 0, 1, 1),
                (0, 0, 0, 0, 0, 0, 0, 0, 1, 0))
        assert fp.calcula_numero_cadeias_montanhas(grid) == 6
        
    def test_7(self):
        grid = ((0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1),
                (0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1),
                (1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1),
                (0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0))
        assert fp.calcula_numero_cadeias_montanhas(grid) == 9
        
    def test_8(self):
        grid = ((1,1,0),(0,1,0),1,(0,1,0),(0,1,1))
        with pytest.raises(ValueError) as excinfo:
            fp.calcula_numero_cadeias_montanhas(grid)
        assert "calcula_numero_cadeias_montanhas: argumento invalido" == str(excinfo.value)
 
class TestCalculaTamanhoVales:
    def test_1(self):
        grid = ((0,0,0),(0,1,0),(1,1,1),(0,1,0),(0,0,1))
        assert fp.calcula_tamanho_vales(grid) == 6
        
    def test_2(self):
        grid = ((0,0,0),(0,1,0),(1,0,1),(0,1,0),(0,0,0))
        assert fp.calcula_tamanho_vales(grid) == 7
        
    def test_3(self):
        grid =  ((1,)*10,)*12
        assert fp.calcula_tamanho_vales(grid) == 0
        
    def test_4(self):
        grid =  ((0,)*13,)*18
        assert fp.calcula_tamanho_vales(grid) == 0

    def test_5(self):
        grid = ((0,0,0),(0,1,1),(1,0,1),(1,1,0),(0,0,0), (0,0,0),(0,1,1),(1,0,1),(1,1,0),(0,0,0),(0,0,0),(0,1,1),(1,0,1),(1,1,0),(0,0,0))
        assert fp.calcula_tamanho_vales(grid) == 21
        
    def test_6(self):
        grid = ((1, 1, 1, 1, 1, 0, 1, 1, 1, 1),
                (1, 0, 1, 0, 0, 0, 0, 0, 1, 1),
                (1, 0, 0, 1, 0, 0, 1, 1, 1, 0),
                (0, 1, 1, 1, 1, 0, 0, 0, 1, 1),
                (0, 1, 1, 1, 0, 0, 0, 0, 1, 0),
                (0, 0, 1, 0, 0, 0, 1, 1, 0, 1),
                (0, 1, 0, 0, 1, 0, 1, 1, 1, 1),
                (1, 1, 1, 0, 0, 0, 0, 1, 1, 0),
                (0, 0, 1, 0, 0, 0, 1, 1, 1, 1),
                (0, 1, 0, 0, 0, 1, 0, 0, 0, 1))
        
        assert fp.calcula_tamanho_vales(grid) == 43
        
    def test_7(self):
        grid = ((1, 1, 1, 1), (1, 1, 0, 1), (1, 0, 1, 0), (0, 1, 1, 0),
                (0, 0, 1, 0), (0, 0, 0, 0), (0, 0, 0, 0), (1, 1, 1, 0),
                (0, 0, 1, 1), (0, 1, 0, 1), (0, 1, 0, 0), (0, 1, 0, 0),
                (0, 1, 1, 0), (1, 0, 1, 1), (0, 0, 1, 1), (0, 1, 1, 1),
                (1, 0, 0, 0), (0, 0, 0, 0), (1, 1, 0, 0), (1, 0, 1, 1))
        assert fp.calcula_tamanho_vales(grid) == 35
        
    def test_8(self):
        grid = ((0,0,0),[0,1,1],(1,0,1),(1,1,0),(0,0,0), (0,0,0),(0,1,1),(1,0,1),(1,1,0),(0,0,0),(0,0,0),(0,1,1),(1,0,1),(1,1,0),(0,0,0))
        with pytest.raises(ValueError) as excinfo:
            fp.calcula_tamanho_vales(grid)
        assert "calcula_tamanho_vales: argumento invalido" == str(excinfo.value)
 

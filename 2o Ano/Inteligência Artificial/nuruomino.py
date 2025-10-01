# nuruomino.py: Template para implementação do projeto de Inteligência Artificial 2024/2025.
# Devem alterar as classes e funções neste ficheiro de acordo com as instruções do enunciado.
# Além das funções e classes sugeridas, podem acrescentar outras que considerem pertinentes.

# Grupo 83:
# 109441 Alexandre Delgado
# 110206 Madalena Yang

from sys import stdin
import numpy as np
from search import depth_first_tree_search, Problem, Node


class Board:
    """Representação interna de um tabuleiro do Puzzle Nuruomino."""

    def __init__(self, board: list, regions: dict, filled_regions: set, unfilled_regions: list, adjacency_lists: dict, parent_node: dict, rank: dict):
        """Construtor da classe Board. O argumento é uma lista de listas
        que representa o tabuleiro."""
        self.board = board
        self.k = len(board)
        self.regions = regions
        self.filled_regions = filled_regions
    
        self.unfilled_regions = unfilled_regions

        self.adjacency_lists = adjacency_lists

        self.parent_node = parent_node
        self.rank = rank

        self.actions_by_region = {} 

      
    
    def adjacent_positions(self, row:int, col:int) -> list:
        """Devolve as posições adjacentes à região, em todas as direções, incluindo diagonais."""

        adjacent_positions = [None] * 8
        nao_cima = nao_baixo = nao_esquerda = nao_direita = False

        '''
        indices:
        0 1 2    X X .    . X X    . . .    . . .  
        3 x 4    X o .    . o X    . o X    X o . 
        5 6 7    . . .    . . .    . X X    X X . 

        '''

        # Se ele nao estiver na primeira linha, podemos adicionar a celula logo acima
        if row != 0:
            nao_cima = True
            adjacent_positions[1] = (col,row-1)

        # Se ele nao estiver na ultima linha, podemos adicionar a celula logo abaixo
        if row != (self.k - 1):
            nao_baixo = True
            adjacent_positions[6] = (col,row+1)
        
        # Etc
        if col != 0:
            nao_esquerda = True
            adjacent_positions[3] = (col-1,row)

        if col != (self.k - 1):
            nao_direita = True
            adjacent_positions[4] = (col+1,row)

        if nao_cima:
            if nao_esquerda:
                adjacent_positions[0] = (col-1,row-1)
            if nao_direita:
               adjacent_positions[2] = (col+1,row-1)

        if nao_baixo:
            if nao_esquerda:
                adjacent_positions[5] = (col-1,row+1)
            if nao_direita:
                adjacent_positions[7] = (col+1,row+1)

        # Ja estao ordenadas 
        return adjacent_positions


    def adjacent_values(self, row: int, col: int) -> list:
        """Devolve os valores das celulas adjacentes à região, em todas as direções, incluindo diagonais."""

        adjacent_positions = self.adjacent_positions(row, col)

        adjacent_values = [] 
        for position in adjacent_positions:
            if position == None:
                adjacent_values.append(position)
            else:
                (x, y) = position
                # Trocar pois y e row e x e col
                adjacent_values.append(self.get_value(y, x))

        return adjacent_values


    def adjacent_positions_no_diag(self, row:int, col:int) -> list:
        """Devolve as posições adjacentes à região, em todas as direções, NAO incluindo diagonais."""

        '''
        . 0 .
        1 x 2
        . 3 .
        '''


        adjacent_positions = [None] * 4

        # Se ele nao estiver na primeira linha, podemos adicionar a celula logo acima
        if row != 0:
            adjacent_positions[0] = (col, row-1)

        # Se ele nao estiver na ultima linha, podemos adicionar a celula logo abaixo
        if row != (self.k - 1):
            adjacent_positions[3] = (col, row+1)
        
        # Etc
        if col != 0:
            adjacent_positions[1] = (col-1, row)

        if col != (self.k - 1):
            adjacent_positions[2] = (col+1, row)

        return adjacent_positions


    def adjacent_values_no_diag(self, row:int, col:int) -> list:
        """Devolve os valores das celulas adjacentes à região, em todas as direções, incluindo diagonais."""
        adjacent_positions_no_diag = self.adjacent_positions_no_diag(row, col)

        adjacent_values_no_diag = [] 
        for (x, y) in adjacent_positions_no_diag:
            # Trocar pois y e row e x e col
            adjacent_values_no_diag.append(self.get_value(y, x))

        return adjacent_values_no_diag

    
    def adjacent_positions_only_diag(self, row: int, col: int) -> list:
        adjacent_positions_only_diag = [None] * 4
        '''
        indices:
        0 . 1    X X .    . X X    . . .    . . .
        . x .    X o .    . o X    . o X    X o .
        3 . 2    . . .    . . .    . X X    X X .

        '''


        # Se ele nao estiver na primeira linha, podemos adicionar a celula logo acima
        # if nao cima
        if row != 0:
            # if nao esquerda
            if col != 0:
                adjacent_positions_only_diag[0] = (col-1,row-1)
            # if nao direita
            if col != (self.k - 1):
               adjacent_positions_only_diag[1] = (col+1,row-1)


        # if nao baixo
        if row != (self.k - 1):
            # if nao esquerda
            if col != 0:
                adjacent_positions_only_diag[3] = (col-1,row+1)
            # if nao direita
            if col != (self.k - 1):
                adjacent_positions_only_diag[2] = (col+1,row+1)


        return adjacent_positions_only_diag



    def adjacent_regions(self, region: str, initial_board: 'Board') -> set:
        """Devolve uma lista das regiões que fazem fronteira com a região enviada no argumento."""
        # Nao incluir diagonais porque 2 pecas iguais podem "tocar-se" na diagonal
        adjacent_regions = set()

        cell_list = self.regions[region]
        initial_board_local = initial_board.board

        for (x, y) in cell_list:
            # cell_list e um set, verificacao e O(1)
            if y != 0 and (x, y-1) not in cell_list:
                #up_value = initial_board.get_value(y-1, x)
                #up_value = initial_board.board[y-1, x]
                up_value = initial_board_local[y-1, x]
                adjacent_regions.add(up_value)
            # o not in cell list e para confirmar que nao estamos a ir ver o valor duma celula da mesma regiao
            if y != (self.k - 1) and (x, y+1) not in cell_list:
                #down_value = initial_board.get_value(y+1, x)
                #down_value = initial_board.board[y+1, x]
                down_value = initial_board_local[y+1, x]
                adjacent_regions.add(down_value)
            if x != 0 and (x-1, y) not in cell_list:
                #left_value = initial_board.get_value(y, x-1)
                #left_value = initial_board.board[y, x-1]
                left_value = initial_board_local[y, x-1]
                adjacent_regions.add(left_value)
            if x != (self.k -1) and (x+1, y) not in cell_list:
                #right_value = initial_board.get_value(y, x+1)
                #right_value = initial_board.board[y, x+1]
                right_value = initial_board_local[y, x+1]
                adjacent_regions.add(right_value)

        return adjacent_regions
        
    
    # Regions affected by a piece and the values on the current board that each adjacent cell from the affected region has
    def affected_regions(self, piece_all_coords: frozenset, current_region: str, problem: 'Nuruomino') -> tuple:
        affected_regions = set()
        # Dicionario que contem, por cada regiao afetada, todas as posicoes afetadas (com coordenadas)
        affected_positions_by_region = {}

        initial_board_local = problem.initial.board.board

        # Para cada celula da peca
        for (x, y) in piece_all_coords:

            # Vejo as adjacent positions (and cache them if needed)
            adjacent_positions = problem.adjacent_positions_cache[ (x,y) ]
            if adjacent_positions == None:
                adjacent_positions = problem.cache_adjacent_positions(x, y)

            adjacent_positions_no_diag = adjacent_positions[0]

            # Para cada posicao adjacente a esta celula vemos qual a regiao, e adicionamos se n for a mesma regiao da peca
            for position in adjacent_positions_no_diag:
                if position == None:
                    continue

                (pos_x, pos_y) = position

                #adjacent_region = problem.initial.board.board[pos_y, pos_x]
                adjacent_region = initial_board_local[pos_y, pos_x]

                if adjacent_region != current_region:
                    affected_regions.add(adjacent_region)

                    if adjacent_region not in affected_positions_by_region:
                        affected_positions_by_region[adjacent_region] = set()

                    affected_positions_by_region[adjacent_region].add( (pos_x, pos_y) )


        return (affected_regions, affected_positions_by_region)

        

    def get_value(self, row:int, col:int) -> int:
        """Devolve o valor preenchido numa determinada posição"""
        return self.board[row, col]
    

    def print(self):
        """Imprime o tabuleiro no formato especificado no enunciado."""
        row_count = 0
        for row in self.board:
            row_count += 1
            value_count = 0
            for value in row:
                value_count += 1 
                if value_count == self.k:
                    if row_count == self.k:
                        print(value, end="")
                    else:
                        print(value, end="\n")
                else:
                    print(value, end="\t")


    # For debug purposes - ajuda a ver
    def only_pieces_print(self):
        """Imprime o tabuleiro no formato especificado no enunciado."""
        row_count = 0
        for row in self.board:
            row_count += 1
            value_count = 0
            for value in row:
                value_count += 1 
                if value_count == self.k:
                    if row_count == self.k:
                        if value in ['L', 'I', 'T', 'S']:
                            print(value, end="")
                        else:
                            print(".", end="")
                    else:
                        if value in ['L', 'I', 'T', 'S']:
                            print(value, end="\n")
                        else:
                            print(".", end="\n")
                else:
                    if value in ['L', 'I', 'T', 'S']:
                        print(value, end="\t")
                    else:
                        print(".", end="\t")



    @staticmethod
    def parse_instance():
        """Lê o test do standard input (stdin) que é passado como argumento
        e retorna uma instância da classe Board.    

        Por exemplo:
            $ python3 pipe.py < test-01.txt

            > from sys import stdin
            > line = stdin.readline().split()
        """

        # ler o tabuleiro e guardar num array do numpy 
        # board e numpy array
        board = np.loadtxt(stdin, dtype=str)
        regions = {}
        parent_node = {}
        rank = {}

        for row in range(board.shape[0]):
            for col in range(board.shape[1]):
                # para nao ficar regions: {np.str_('1'): [(0, 0), (1, 0), (0, 1), (0, 2)], np.str_('2'):
                # ficamos com regions: {'1': [(0, 0), (1, 0), (0, 1), (0, 2)], '2': ...} como tinhamos
                cell = str(board[row, col])
                # Se e uma regiao nova
                if cell not in regions:
                    regions[cell] = []
                    #regions[cell] = set()
                    parent_node[cell] = cell
                    rank[cell] = 0

                regions[cell].append((col, row))
                #regions[cell].add((col,row))

        

        '''
        Regions e deste formato:
        '1': [(1,1), (1,2), ..., (x,y)]
        '2': ...
        '''

         
        # filled regions starts empty, and so do linked and adjacency lists
        return Board(board, regions, set(), [], {}, parent_node, rank)
   

    
    def change_cell(self, row: int, col: int, value: str):
        self.board[row][col] = value


    def update_region(self, region: str):
        self.filled_regions.add(region)
        self.unfilled_regions.remove(region)
    

    # With path compression and still recursive
    def find(self, parent_node, x):
        if parent_node[x] != x:
            parent_node[x] = self.find(parent_node, parent_node[x])
        return parent_node[x]
   
    # With ranking
    def union(self, parent_node, rank, x, y):
        root_x = self.find(parent_node, x)
        root_y = self.find(parent_node, y)

        if root_x == root_y:
            return
        if rank[root_x] > rank[root_y]:
            parent_node[root_y] = root_x
        elif rank[root_x] < rank[root_y]:
            parent_node[root_x] = root_y
        else:
            parent_node[root_x] = root_y
            rank[root_y] = rank[root_y] + 1
    


    def set_actions_by_region(self, actions_by_region: dict):
        self.actions_by_region = actions_by_region

    def set_sorted_unfilled_regions(self, sorted_unfilled_regions: list):
        self.unfilled_regions = sorted_unfilled_regions


class NuruominoState:
    state_id = 0

    def __init__(self, board: Board):
        self.board = board
        self.id = NuruominoState.state_id
        NuruominoState.state_id += 1


    def __lt__(self, other):
        """ Este método é utilizado em caso de empate na gestão da lista
        de abertos nas procuras informadas. """
        return self.id < other.id



class Nuruomino(Problem):

    def __init__(self, board: Board):
        """O construtor especifica o estado inicial."""
        self.initial = NuruominoState(board) 

        self.all_pieces = {

                'L': [ [[1,0],[1,0],[1,1]], [[1,1],[1,0],[1,0]], [[1,1],[0,1],[0,1]], [[0,1],[0,1],[1,1]], [[1,1,1],[1,0,0]], [[1,1,1],[0,0,1]], [[0,0,1],[1,1,1]], [[1,0,0],[1,1,1]] ],

                'I': [ [[1],[1],[1],[1]], [[1,1,1,1]] ],

                'T': [ [[1,1,1], [0,1,0]], [[0,1,0], [1,1,1]], [[1,0],[1,1],[1,0]], [[0,1],[1,1],[0,1]] ], 

                'S': [ [[0,1,1], [1,1,0]], [[1,1,0],[0,1,1]], [[0,1],[1,1],[1,0]], [[1,0],[1,1],[0,1]] ]
                }

        self.all_letters = {'L', 'I', 'T', 'S'}
 
        '''
        {
        '1': [ (L, 0, (1, 0)), ...]
        'region': [ (letter, index, starting_coordinate), ...]
        }
        '''

        self.actions_by_region = {}

        for region in board.regions:
            self.actions_by_region[region] = self.pieces_in_region(self.initial, int(region))

        # Vamos ordenar as unfilled regions da que tem menos jogadas possiveis para a que tem mains, de modo a criar o minimo de nos desde o inicio
        # reverse = False para ficar por ascending order
        sorted_unfilled_regions = sorted(self.actions_by_region, key=lambda k: len(self.actions_by_region[k]), reverse=False)

        board.set_sorted_unfilled_regions(sorted_unfilled_regions)

        board.set_actions_by_region(self.actions_by_region)

        self.adjacent_positions_cache = {}
        for region_cells in board.regions.values():
            for cell in region_cells:
                self.adjacent_positions_cache[cell] = None


        self.adjacent_regions_by_region = {}
        for region in board.regions.keys():
            self.adjacent_regions_by_region[region] = board.adjacent_regions(region, self.initial.board)

        self.affected_regions_by_piece_cache = {}
        for region, action_list in self.actions_by_region.items():
            for action in action_list:
                (_, _, piece_all_coords) = action
                self.affected_regions_by_piece_cache[piece_all_coords] = None
 
        self.piece_validation_cache = {}   


    def actions(self, state: NuruominoState):
        """Retorna uma lista de ações que podem ser executadas a
        partir do estado passado como argumento."""

        possible_actions = []

        # Se todas as regioes estiverem preenchidas, nao ha mais acoes possiveis
        if len(state.board.unfilled_regions) == 0:
            return possible_actions

        region = state.board.unfilled_regions[0]
        
        # So vemos pecas que ja sabemos que cabem nessa regiao
        action_list = state.board.actions_by_region[region]
        for action in action_list:
            (letra, index, piece_all_coords ) = action 

            piece = self.all_pieces[letra][index] 
    
             
            # Sempre que quisermos chamar a funcao das ilhas temos que dar deep copy das linked e adjacency lists
            new_adjacency_lists = {}

            # Estes nao precisam de deep copy pois os values sao strings, que sao immutable - shallow copy basta
            new_parent_node = state.board.parent_node.copy() 
            new_rank = state.board.rank.copy()

            
            # Deep copy adjacency lists - este precisa pois os values sao sets e sets sao mutable
            for key, value in state.board.adjacency_lists.items():
                # set() function retorna novas referencias quando passamos ja um set como argumento e o set e igual
                new_adjacency_lists[key] = set(value)
            
           
            
            # A cache is useful here because, even though nao pareca at first, cada peca pode de facto ser checked mais do que uma vez apesar das suas piece_all_coords serem unique, porque cada node chama isto, e pode haver acoes iguais em nodes paralelos
            # Ou seja, varios nodes podem tar a verificar a mesma acao entao a cache da jeito
            #affected_regions_and_values = state.board.affected_regions(piece_all_coords, region, self)
            affected_regions_and_positions = self.affected_regions_by_piece_cache[piece_all_coords]
            if affected_regions_and_positions == None:
                affected_regions_and_positions = self.cache_affected_regions(piece_all_coords, region, state)


            affected_positions_by_region = affected_regions_and_positions[1]



            # A funcao vai alterar o linked e adjacency lists por referencia
            if self.doesnt_create_island(state, state.board, region, piece, piece_all_coords, affected_positions_by_region, new_adjacency_lists, new_parent_node, new_rank) == True:
            


                # Compute filled_touches (connections)
                filled_touches = 0
                for (x, y) in piece_all_coords:
                    for dx, dy in [(1,0), (-1,0), (0,1), (0,-1)]:  # Orthogonal only
                        nx, ny = x+dx, y+dy
                        if 0<=nx<state.board.k and 0<=ny<state.board.k:
                            if state.board.board[ny, nx] in self.all_pieces:
                                filled_touches += 1









                # Forward checking from here on out
                new_board = state.board.board.copy()
                for (x, y) in piece_all_coords:
                    new_board[y, x] = letra


                new_filled_regions = set(state.board.filled_regions)
                new_unfilled_regions = state.board.unfilled_regions.copy()

               
                # nao e preciso copiar o board.regions pois esse e igual durante o programa todo
                # adjacency lists, parent node e rank ja foram deep copied no actions la em cima e têm que ser feitos la pois e preciso fzr deep copy por cada chamada da funcao das islands
                new_board_object = Board(new_board, state.board.regions, new_filled_regions, new_unfilled_regions, new_adjacency_lists, new_parent_node, new_rank)
                #new_board_object = Board(new_board, state.board.regions, new_filled_regions, new_unfilled_regions, adjacency_lists, parent_node, rank)

                new_board_object.update_region(region)
                
                new_board_object.set_actions_by_region(state.board.actions_by_region)

                actions_by_affected_region = {}

                #affected_regions = self.adjacent_regions_by_region[region]
                affected_regions = affected_regions_and_positions[0]
                

                action_invalidated = False 
                # Better if i remove tons of actions - better start just with an empty set and ir adicionando
                # usamos o new_board_object pois tem as updated filled regions
                # Se ja esta filled n vale a pena copiarmos
                for r in affected_regions:
                    if r in new_board_object.filled_regions:
                        continue

                    # Se estiver nas regioes afetadas pela jogada que fiz, atualizo as acoes possiveis pa esta regiao
                    # affected_regions e um set, lookup e O(1)
                    new_actions_by_r = set()

                    actions_by_r = new_board_object.actions_by_region[r]
                    for future_action in actions_by_r:

                        (future_letter, future_index, future_piece_all_coords) = future_action

                        future_affected_regions_and_positions = self.affected_regions_by_piece_cache[future_piece_all_coords]
                        if future_affected_regions_and_positions == None:
                                future_affected_regions_and_positions = self.cache_affected_regions(future_piece_all_coords, r, state)

                        future_affected_positions_by_region = future_affected_regions_and_positions[1]




                        # We dont need future_affected_positions_by_region as part of the key because it directly depends on future_piece_all_coords and depends only on that (and the initial board which doesnt change)
                        # So since we have future piece all coords in the key we're fine. Plus the future affected positions by region is a dict so we wouldnt be able to store it in a dictionary key
                        #key = (new_board.tobytes(), future_letter, future_piece_all_coords)
                        # .get() provides a fallback value se a key nao estiver no dicionario
                        #validate_piece = self.piece_validation_cache.get(key, None)

                        #if validate_piece == None:
                            #validate_piece = self.cache_piece_validation(new_board_object, future_letter, future_piece_all_coords, future_affected_positions_by_region)

                        #if validate_piece == True:
                        if self.validate_piece_placement(new_board_object, future_letter, future_piece_all_coords, future_affected_positions_by_region) == True:
                            # Sempre que quisermos chamar a funcao das ilhas temos que dar deep copy das linked e adjacency lists
                            future_adjacency_lists = {}

                            # Estes nao precisam de deep copy pois os values sao strings, que sao immutable - shallow copy basta
                            #future_parent_node = state.board.parent_node.copy() 
                            future_parent_node = new_parent_node.copy()
                            #future_rank = state.board.rank.copy()
                            future_rank = new_rank.copy()

                            
                            # Deep copy adjacency lists - este precisa pois os values sao sets e sets sao mutable
                            #for key, value in state.board.adjacency_lists.items():
                            for key, value in new_adjacency_lists.items():
                                # set() function retorna novas referencias quando passamos ja um set como argumento e o set e igual
                                future_adjacency_lists[key] = set(value)

                            future_piece = self.all_pieces[future_letter][future_index]

                            if self.doesnt_create_island(state, new_board_object, r, future_piece, future_piece_all_coords, future_affected_positions_by_region, future_adjacency_lists, future_parent_node, future_rank) == True:

                                # Ive computed new info for adj lists, parent nodes and ranks so ill store it

                                new_actions_by_r.add(future_action)

                    
                    if new_actions_by_r == set():
                        action_invalidated = True


                    # So faco o dictionary access once para poupar tempo
                    actions_by_affected_region[r] = new_actions_by_r
                    
                if action_invalidated == False: 
                    possible_actions.append((region, letra, index, piece_all_coords, new_adjacency_lists, new_parent_node, new_rank, affected_regions, actions_by_affected_region, new_board_object, filled_touches))
                    #possible_actions.append((region, letra, index, piece_all_coords, adjacency_lists, parent_node, rank, affected_regions, actions_by_affected_region, new_board_object, filled_touches))



        # Sort actions

        # Por primeiro as ações que tocam em regiões sem peça e que tocam em menos regiões
        
        # Ascending, therefore reverse False
        #possible_actions = sorted(possible_actions, key=lambda k: sum(1 for r in k[7] if r in state.board.unfilled_regions), reverse=False)

        # Ascending, therefore no reverse
        #possible_actions = sorted(possible_actions, key=lambda k: len(k[7]))

        # Add to action tuple (index 10)
        #possible_actions.append((..., filled_touches))
        possible_actions.sort(key=lambda k: (
                    #sum(len(k[8][r]) for r in k[7] if r in state.board.unfilled_regions), #total remaining actions por regiao afetada
                    min((len(k[8][r]) for r in k[7] if r in state.board.unfilled_regions), default=float('inf')), #acoes que afetam regioes vazias com menos opcoes left para acoes primeiro
                    #sum(1 for r in k[7] if r in state.board.unfilled_regions), #acoes com menos regioes afetadas vazias primeiro
                    len(k[7]), #acoes com menos regioes afetadas primeiro
                    -k[10]  # filled_touches (descending)
                    #sum(1 for r in k[7] if len(k[8].get(r, [])) == 1), # acoes q afetam regioes com apenas uma opcao primeiro
                    ))

        return possible_actions


    def result(self, state: NuruominoState, action):
        """Retorna o estado resultante de executar a 'action' sobre
        'state' passado como argumento. A ação a executar deve ser uma
        das presentes na lista obtida pela execução de
        self.actions(state)."""


        #(region, action_letter, action_piece_index, piece_all_coords, adjacency_lists, parent_node, rank, affected_regions, actions_by_affected_region, new_board_object) = action
        (_, _, _, _, _, _, _, affected_regions, actions_by_affected_region, new_board_object, _) = action

        new_actions_by_region = {}

        for r, actions_by_r in state.board.actions_by_region.items():
            if r in new_board_object.filled_regions:
                continue

            if r in affected_regions:
                new_actions_by_region[r] = actions_by_affected_region[r]

            else:
                new_actions_by_region[r] = set(actions_by_r)

        
        new_board_object.set_actions_by_region(new_actions_by_region)

        return NuruominoState(new_board_object)




    def goal_test(self, state: NuruominoState):
        """Retorna True se e só se o estado passado como argumento é
        um estado objetivo. Deve verificar se todas as posições do tabuleiro
        estão preenchidas de acordo com as regras do problema."""
        if len(state.board.filled_regions) != len(self.initial.board.regions):
            return False

        
        if state.board.adjacency_lists != {}:
            return False

        # Caso adjacency_lists seja vazia, ja nao ha regioes a preencher, vejo se estao todas conectadas

        # Fazer o find para cada node e mt barato pois o union find esta com path compression e ranking
        # (O(α(n)), where α is the inverse Ackermann function — so small it's practically constant for any realistic input)
        # Por isso nao e preciso nenhum linked regions dictionary onde cada region aponta para a representante

        root = state.board.find(state.board.parent_node, next(iter(state.board.parent_node)))
        return all(root == state.board.find(state.board.parent_node, x) for x in state.board.parent_node)


    def h(self, node: Node):
        """Improved heuristic function for Nuruomino puzzle"""
        board = node.state.board
        
        unfilled_count = len(board.unfilled_regions)
        
        # Connectivity penalty
        component_roots = set()
        for region in board.filled_regions:
            root = board.find(board.parent_node, region)
            component_roots.add(root)
        
        # Disconnected components that are isolated
        isolated_components = 0
        for root in component_roots:
            if root not in board.adjacency_lists:
                isolated_components += 1
        
        # Heuristica = unfilled regions + penalty for isolated components
        return unfilled_count + isolated_components
 

    

    def pieces_in_region(self, state: NuruominoState, region: int) -> set:#-> list:
        """Verifica que peças podem ser colocada na região indicada."""

        # [ (letra, indice letra, starting coordinate), ... ]
        pieces_possible = set()

        # =================================================================================
        # Fazer matriz regiao
        region_cells = self.initial.board.regions[str(region)]
        x = []
        y = []
        for i in range(len(region_cells)):
            x.append(region_cells[i][0])
            y.append(region_cells[i][1])


        # matriz para guardar o formato da regiao
        n_cols = max(x) - min(x) + 1
        n_lines = max(y) - min(y) + 1

        matrix = np.zeros((n_lines, n_cols), dtype=int)
        changed = 0
        # preencher a matriz com os valores da regiao
        for row in range(len(matrix)):
            for col in range(len(matrix[row])):
                # Os min(x) e min(y) traduzem as coordenadas da matriz nas coordenadas do board
                if (col + min(x), row + min(y)) in region_cells:
                    matrix[row][col] = 1
                    changed += 1
                if (changed == len(region_cells)):
                    break
            if (changed == len(region_cells)):
                    break
            
        # =================================================================================

        
        for letra in self.all_pieces:
            for piece_index in range(len(self.all_pieces[letra])):
                
                piece = self.all_pieces[letra][piece_index]

                # Se a peca tiver mais linhas que a regiao, nao cabe de certeza
                # Se a peca ocupar mais colunas que a regiao, tambem nao cabe de certeza
                if len(piece) > len(matrix) or len(piece[0]) > len(matrix[0]):
                    continue

                coordinates = self.fit_piece(piece, matrix, min(x), min(y), state, letra)
                
                # Para cada starting coordenada possivel organiza a informacao e adiciona a lista final
                for (board_x, board_y) in coordinates:

                    piece_all_coords = set()

                    for row in range(len(piece)):
                        for col in range(len(piece[row])):
                            if piece[row][col] == 0:
                                continue
                            piece_all_coords.add( (board_x+col, board_y+row) )
                    

                    pieces_possible.add( (letra, piece_index, frozenset(piece_all_coords)) )

        return pieces_possible


    # Ve se a peca comparando linha a linha, como se a peca estivesse a descer para a regiao e depois a andar para a diretia
    def fit_piece(self, piece: list, matrix: np.array, adjust_x: int, adjust_y: int, state: NuruominoState, letra: str) -> list: 
        n_piece_rows = len(piece)
        n_piece_cols = len(piece[0])
            
        n_matrix_rows = len(matrix)
        n_matrix_cols = len(matrix[0])

        coords = []

        # Loop que vai andando para a direita na regiao  
        for j in range(n_matrix_cols - n_piece_cols + 1):

            # first slice selects all rows, second selects desired columns, so e possivel porque matrix e um numpy array
            smaller_matrix = matrix[:, j : j + n_piece_cols]

            # Loop que vai andando para baixo na regiao
            for i in range(n_matrix_rows - n_piece_rows + 1):
                # Explicacao do range: se a peca tiver 3 rows e a matriz 3 rows, so ha 1 opcao, n da para descer mais senao vai out of bounds, logo 3 - 3 + 1 = 1 opcao. se a peca tiver 3 rows e a matriz 4 rows entao ha 2 opcoes, a em que a peca ta em cima e a que a peca desce uma row, logo 4 - 3 + 1 = 2 (o 1 e para certificar que o loop corre sempre pelo menos 1 vez)


                # Loop que compara linha a linha
                
                # Pegamos nas linhas da peca e nas primeiras linhas correspondentes na matriz e vemos se cabe
                # Se não, passamos uma linha na matriz para baixo
                for piece_row, matrix_row in zip(piece, smaller_matrix[i : i + n_piece_rows]):

                    # Loop que compara celula a celula
                    for piece_cell, matrix_cell in zip(piece_row, matrix_row):
                        fits = True
                        
                        # Se a peca tiver uma celula que nao esta na regiao, entao nao cabe neste conjunto de linhas
                        if matrix_cell == 0 and piece_cell == 1:
                            fits = False
                            break           # Da break e por isso vai ver se noutro conjunto de rows da regiao ja cabe
                    # Temos que dar 2 breaks, nao ir ver a proxima linha do conjunto (piece, matrix) mas sim passar para o proximo i para passarmos para o proximo conjunto de rows da matriz regiao
                    if fits == False:
                        break

                # Se cabe, adicionamos a lista de coordenadas
                if fits == True:

                    # Os ajust_x e adjust_y (min(x) e min(y) da funcao pai) traduzem as coordenadas da peca na matriz nas coordenadas da peca no board
                    board_x = j + adjust_x
                    board_y = i + adjust_y

                    coords.append( (board_x, board_y) )
                    

        return coords


    def validate_piece_placement(self, board: Board, letra: str, piece_all_coords: frozenset, affected_positions_by_region: dict) -> bool:

        touches_same_letter = False
        makes_tetramino_O = False

        # Improves lookups slightly
        board_local = board.board
       
        for affected_region, affected_positions in affected_positions_by_region.items():
            for (pos_x, pos_y) in affected_positions:

                value = board_local[pos_y, pos_x]
                if value == letra:
                    touches_same_letter = True
                    return False





 
        # For simplicity
        #letras = self.all_pieces
        letras = self.all_letters

        # Por cada celula na piece que seja 1 (faz parte da peca) vamos buscar posicoes e valores adjacentes para ver restricoes
        # Para ver o tetramino O temos mesmo que percorrer o piece_all_coords de novo pois o affected regions e affected positions nunca veem diagonais
        for (col, row) in piece_all_coords:

            # Get adjacent positions and cache it se necessario
            adjacent_positions = self.adjacent_positions_cache[ (col,row) ]
            # In case it isnt in the cache yet, we calculate it and store it
            if adjacent_positions == None:
                adjacent_positions = self.cache_adjacent_positions(col, row)

            # Vemos se a peca, ao ser colocada, forma um quadrado 2x2 (peca de tetramino O)

            '''
            0 1 2    X X .    . X X    . . .    . . .  
            3 x 4    X o .    . o X    . o X    X o . 
            5 6 7    . . .    . . .    . X X    X X . 

            'corner': [ (position, value), ... ]
            where position are (x,y) coordinates on the board

            adjacent positions e tuplo ( [no diag], [only diag] )
            
              0           1

            . 0 .       0 . 1       10  00  11
            1 x 2       . x .       01  xx  02
            . 3 .       3 . 2       13  03  12

            '''


            corners = { 'upper_left': [ adjacent_positions[0][1],\
                                        adjacent_positions[1][0],\
                                        adjacent_positions[0][0] ],

                        'upper_right': [ adjacent_positions[0][0],\
                                        adjacent_positions[1][1],\
                                        adjacent_positions[0][2] ],

                        'lower_right': [ adjacent_positions[0][2],\
                                        adjacent_positions[1][2],\
                                        adjacent_positions[0][3] ],

                        'lower_left': [ adjacent_positions[0][3],\
                                        adjacent_positions[1][3],\
                                        adjacent_positions[0][1] ]
                       }


            for corner in corners.values():
                if corner[0] == None or corner[1] == None or corner[2] == None:
                    continue

                hits = 0
                for (x, y) in corner:
                    if (x, y) in piece_all_coords or board_local[y, x] in letras:
                        hits+=1

                if hits == 3:
                    makes_tetramino_O = True
                    return False






        if touches_same_letter == False and makes_tetramino_O == False:
            # A peca passada e valida na posicao indicada
            return True
        

        return False
        

    
    #def doesnt_create_island(self, state: NuruominoState, region: str, piece: list, piece_all_coords: frozenset, affected_positions_by_region: dict, adjacency_lists: dict, parent_node: dict, rank: dict) -> bool:
    def doesnt_create_island(self, state: NuruominoState, board_to_use: Board, region: str, piece: list, piece_all_coords: frozenset, affected_positions_by_region: dict, adjacency_lists: dict, parent_node: dict, rank: dict) -> bool:
        
        touches_piece = False

        #board_local = state.board.board
        board_local = board_to_use.board 

        adjacent_touching_regions = set()
        adjacent_filled_regions = set()
        adjacent_unfilled_regions = set()
       
        for affected_region, affected_positions in affected_positions_by_region.items():
            for (pos_x, pos_y) in affected_positions:

                #value = state.board.board[pos_y, pos_x]
                value = board_local[pos_y, pos_x]

                # Se o valor adjacente e uma letra entao a peca esta a tocar em algo
                #if value in self.all_pieces:
                if value in self.all_letters:
                    adjacent_touching_regions.add(affected_region)
                    touches_piece = True

            if affected_region in state.board.unfilled_regions:
                adjacent_unfilled_regions.add(affected_region)
            else:
                adjacent_filled_regions.add(affected_region)



        # Se a peca nao tocar em nenhuma peca e todas as regioes adjacentes ja tem peca entao criamos uma ilha
        criara_ilha = (touches_piece == False) and (adjacent_unfilled_regions == set())

        if criara_ilha == True:
            return False

        
        # Criar entry nova no adjacency_lists ou dar update a adjecency list do grupo
        main_region = state.board.find(parent_node, region) 
        try:
            adjacency_lists[main_region].update(adjacent_unfilled_regions)
        # Caso ela nao exista, inicializamos com set vazio e damos update como queremos
        except KeyError:
            adjacency_lists[main_region] = set()
            adjacency_lists[main_region].update(adjacent_unfilled_regions)
        
        # Para cada regiao com uma peca a tocar na peca que quero por, junto os aglomerados
        for touching_region in adjacent_touching_regions:
     
            # Preciso de fazer isto every iteration para a region pois o representative pode mudar no evento duma union
            region_representative = state.board.find(parent_node, region)
            touching_region_representative = state.board.find(parent_node, touching_region)

            # So vale a pena fazer union se as duas regioes ainda nao pertencerem ao mesmo grupo
            if region_representative != touching_region_representative:

                # Dou merge das duas adjacency lists e apago uma delas
                if touching_region_representative in adjacency_lists:

                    # Neste caso, o union vai juntar o touching_region ao region
                    if rank[region_representative] > rank[touching_region_representative]:
                        adjacency_lists[region_representative].update( adjacency_lists[touching_region_representative] )
                        del adjacency_lists[touching_region_representative]
                    # Caso contrario, o union junta o region ao touching region - logo trocamos os acessos ao adjacency lists
                    # Caso os ranks sejam iguais, o union faz o mesmo, junta o region ao touching region
                    elif rank[region_representative] <= rank[touching_region_representative]:
                        adjacency_lists[touching_region_representative].update( adjacency_lists[region_representative] )
                        del adjacency_lists[region_representative]

                # E dou union as regioes
                state.board.union(parent_node, rank, region, touching_region)



        # Limpar as adjacency_lists
        # - remover a regiao atual de todas as adjacency lists
        # - remover adjacency lists que fiquem vazias

        empty_adjacency_lists = set()
        for root_region in adjacency_lists:
            # Discard nao da erro se o elemento nao estiver la e e O(1) logo nao precisamos de fazer if region in regions_to_be_filled
            adjacency_lists[root_region].discard(region)
 
            if adjacency_lists[root_region] == set():
                empty_adjacency_lists.add(root_region)


        # Assim fica O(y) onde y e o numero de adjacency lists a remover. Se tentassemos remover estas adjacency lists durante o for loop acima teriamos que converter o adjacency_lists para list o que seria O(x) onde x seria o numero de root regions totais, nao so as para remover, o que seria sempre maior (no pior pior caso e so igual)
        for root_region in empty_adjacency_lists:
            del adjacency_lists[root_region]



        # Isolation check after updating the union find structures

        # Neste caso ja nao é preciso ja so falta uma jogada (??)
        #if len(state.board.unfilled_regions) == 1:
        if adjacency_lists == {}:
            return True
        
        
        #representatives = set()
        for r in parent_node:
            root = state.board.find(parent_node, r)
            # Se o root for igual a regiao entao e so uma regiao que ainda nao foi juntada a ninguem entao claro que nao esta nas adj lists
            if root != r:
                #representatives.add(root)

                if root not in adjacency_lists:
                    return False

        
        #for r in representatives:
            # Se houver algum representante sem entrada nas adjacency list significa que essa arvore esta isolada (esse aglomerado no tabuleiro) - logo a acao que fez isto acontecer nao e valida
            #if r not in adjacency_lists:
                #return False



        return True



    
    def cache_adjacent_positions(self, x: int, y: int) -> list:
        cell = (x, y)
        board = self.initial.board
        # Compute it
        adjacent_positions = (board.adjacent_positions_no_diag(y, x), board.adjacent_positions_only_diag(y, x))
        
        # Store and return it
        self.adjacent_positions_cache[cell]  = adjacent_positions
        return adjacent_positions
    
    def cache_affected_regions(self, piece_all_coords: frozenset, region: str, state: NuruominoState) -> tuple:

        affected_regions_and_positions = state.board.affected_regions(piece_all_coords, region, self)

        self.affected_regions_by_piece_cache[piece_all_coords] = affected_regions_and_positions
        return affected_regions_and_positions

    def cache_piece_validation(self, board: Board, letter: str, piece_all_coords: set, affected_positions_by_region: dict) -> bool:
        piece_validation = self.validate_piece_placement(board, letter, piece_all_coords, affected_positions_by_region)
        key = (board.board.tobytes(), letter, piece_all_coords)

        self.piece_validation_cache[key] = piece_validation
        return piece_validation

    
# Ler grelha do figura 1a:
board = Board.parse_instance()

# Criar uma instância de Nuruomino:
problem = Nuruomino(board)

goal_node = depth_first_tree_search(problem)

goal_node.state.board.print()




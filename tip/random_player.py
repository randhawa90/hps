import sys
import numpy as np
from random import choice

def adding_avail(board_info,player):
    bd = []
    plwt = range(1,13)
    for e in board_info:
        if e[1] == 0:
            bd.append(e[0])
        if e[2] == player:
            plwt.remove(e[1])
    return plwt,bd

def calc_torque(board_info,grav_center,support):
    board = np.vstack((board_info,grav_center))
    torque = 0
    for e in board:
        torque = torque + (e[0] - support) * e[1]
    return torque

def tipping(board_info,grav_center,support1,support2):
    tip = False
    torque1 = calc_torque(board_info,grav_center,support1)
    torque2 = calc_torque(board_info,grav_center,support2)
    if torque1 < 0 and torque2 < 0:
        tip = True
    if torque1 > 0 and torque2 > 0:
        tip = True
    return tip

def get_possible_move(p_move,board_info,grav_center,support1,support2):
    possible_move = []
    for e in p_move:
        board_tmp = board_info
        board_tmp[e[0]+15][1] = 0
        board_tmp[e[0]+15][2] = 0
        if not tipping(board_tmp,grav_center,support1,support2):
            possible_move.append(e)
    return possible_move

#---------------------
mode = int(sys.argv[1])
player = int(sys.argv[2])
time = float(sys.argv[3])
board_info = np.loadtxt('board.txt')
grav_center = np.array([0,3,0])
support1 = -3
support2 = -1
if mode == 1:
# adding mode
    # find available weights and places
    plwt,bd = adding_avail(board_info,player)
    # choose a random weight
    rand_wt = choice(plwt)
    # place it as left as possible
    for e in bd:
        possible_move = [e,rand_wt,player]
        board_tmp = np.vstack((board_info,possible_move))
        if not tipping(board_tmp,grav_center,support1,support2):
            break
    # output    
    print str(int(possible_move[0])) + ' ' + str(int(possible_move[1]))
        
if mode == 2:
# removing mode
    p1_move = []
    p2_move = []
    for e in board_info:
        if e[1] != 0:
            if e[2] != 2:
                p1_move.append([int(e[0]),int(e[1])])
            else:
                p2_move.append([int(e[0]),int(e[1])])
                
    if player == 1:
        if p1_move != []:
            possible_move = get_possible_move(p1_move,board_info,grav_center,
                                              support1,support2)
            # if no notipping move, still make a move
            if possible_move == []:
                possible_move.append(p1_move[0])
        else:
            possible_move = get_possible_move(p2_move,board_info,grav_center,
                                              support1,support2)
            if possible_move == []:
                possible_move.append(p2_move[0])

    if player == 2:
        possible_move = get_possible_move(p1_move + p2_move,board_info,
                                          grav_center,support1,support2)
        if possible_move == []:
            possible_move.append((p1_move + p2_move)[0])
    
    # output
    rand_move = choice(possible_move)
    print str(int(rand_move[0])) + ' ' + str(int(rand_move[1]))




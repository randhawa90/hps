import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.IOException;

import javax.swing.SwingWorker;


public class Hunter extends PlayerServer implements EvasionListener {

  public Hunter(EvasionModel model, int N, int W, long totalTime, int portNumber) {
    super(model, N, W, totalTime, portNumber);
    // TODO Auto-generated constructor stub
  }

  public void make_move() throws IOException {
    send_details();
    char[] move = get_move().toCharArray();
    if (move[0] == 'T') {
      model.hunterTimeOver();
    }
    else {
      model.hunterMove(convertToMove(move));
    }
  }

  private HunterMove convertToMove(char[] move) {
    HunterMove convertedMove = new HunterMove();
    if (move.length < 2) {
      convertedMove.move = hunterDirection;
      return convertedMove;
    }
    switch (move[0]+move[1]) {
      case 'N'+'E':
        convertedMove.move = HunterMoves.NE;
        break;

      case 'N'+'W':
        convertedMove.move = HunterMoves.NW;
        break;

      case 'S'+'E':
        convertedMove.move = HunterMoves.SE;
        break;

      case 'S'+'W':
        convertedMove.move = HunterMoves.SW;
        break;
        
      default:
        convertedMove.move = hunterDirection;
        break;
    }
    if (move.length <= 2) {
      return convertedMove;
    }
    if (move[2] == 'w') {
      if (move[3] == 'x') {
        switch (convertedMove.move) {
          case NW:
            convertedMove.move = HunterMoves.NWwx;
            break;

          case NE:
            convertedMove.move = HunterMoves.NEwx;
            break;

          case SW:
            convertedMove.move = HunterMoves.SWwx;
            break;

          case SE:
            convertedMove.move = HunterMoves.SEwx;
            break;

          default:
            break;
        }
        int wallNum = 0;
        if (move.length > 4 && isNum(move[4])) {
          wallNum = (move[4] - '0');
          if (move.length > 5 && isNum(move[5])) {
            wallNum *= 10;
            wallNum += (move[5] - '0');
          }
          convertedMove.wallNumber = wallNum;
        }
      }
      else {
        switch (convertedMove.move) {
          case NW:
            convertedMove.move = HunterMoves.NWw;
            break;

          case NE:
            convertedMove.move = HunterMoves.NEw;
            break;

          case SW:
            convertedMove.move = HunterMoves.SWw;
            break;

          case SE:
            convertedMove.move = HunterMoves.SEw;
            break;

          default:
            break;
        }
        int x1 = 0;
        int y1 = 0;
        int x2 = 0;
        int y2 = 0;
        int index = 5;
        x1 += move[4] - '0';
        while (isNum(move[index])) {
          x1 *= 10;
          x1 += move[index++] - '0';
        }
        index++;
        while (isNum(move[index])) {
          y1 *= 10;
          y1 += move[index++] - '0';
        }
        index += 3;
        while (isNum(move[index])) {
          x2 *= 10;
          x2 += move[index++] - '0';
        }
        index++;
        while (isNum(move[index])) {
          y2 *= 10;
          y2 += move[index++] - '0';
        }
        convertedMove.start = new Point(x1,y1);
        convertedMove.end = new Point(x2,y2);
      }
    }
    return convertedMove;
  }

  boolean isNum(char c) {
    return c >= '0' && c <= '9';
  }
  
  @Override
  public void hunter_moved(HunterMove move, String message) throws IOException {
    move_counter++;
    if (moves_to_next_wall > 0) {
      moves_to_next_wall--;
    }
    hunterPosition = move.position;
    if (move.madeWall) {
      walls.add(new Line2D.Float(move.start, move.end));
      moves_to_next_wall = N;
    }
    else if (move.deletedWall) {
      walls.remove(move.wallNumber-1);
    }
    hunterDirection = move.move;
    if ((move_counter%2) == 1) {
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          make_move();
          return 0;
        }
      }.execute();
    }
  }

  @Override
  public void prey_moved(Point2D move) throws IOException {
    preyPosition = move;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        make_move();
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_caught(long no_of_moves) {
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        close_conn_game_over("You Won!!!");
        return 0;
      }
    }.execute();
  }

  @Override
  public void game_started(String hunterName, String preyName) {
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        make_move();
        return 0;
      }
    }.execute();
  }

  @Override
  public void game_reset() {

  }

  @Override
  public void time_over(long no_of_moves, final char player) {
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        switch (player) {
          case 'h':
            close_conn_game_over("You Timed Out!!!");
            break;

          case 'p':
            close_conn_game_over("Prey Timed Out!!!");
            break;

          default:
            break;
        }
        return 0;
      }
    }.execute();
  }

  @Override
  public String getName() {
    return name;
  }

}

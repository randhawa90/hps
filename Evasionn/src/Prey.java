import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.IOException;

import javax.swing.SwingWorker;


public class Prey extends PlayerServer implements EvasionListener {

  public Prey(EvasionModel model, int N, int W, long totalTime, int portNumber) {
    super(model, N, W, totalTime, portNumber);
  }
  
  public void make_move() throws IOException {
    send_details();
    char[] move = get_move().toCharArray();
    if (move[0] == 'T') {
      model.preyTimeOver();
    }
    else {
      model.preyMove(convertToMove(move));
    }
  }

  @Override
  public void hunter_moved(final HunterMove move, String message) throws IOException {
    // TODO Auto-generated method stub
    move_counter++;
    if (moves_to_next_wall > 0) {
      moves_to_next_wall--;
    }
    hunterPosition = move.position;
    if (move.madeWall) {
      moves_to_next_wall = N;
      walls.add(new Line2D.Float(move.start, move.end));
    }
    else if (move.deletedWall) {
      walls.remove(move.wallNumber-1);
    }
    hunterDirection = move.move;
    if ((move_counter%2) == 0) {
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
  public void prey_moved(Point2D move) {
    preyPosition = move;
  }

  @Override
  public void prey_caught(long no_of_moves) {
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        close_conn_game_over("You Lost!!!");
        return 0;
      }
    }.execute();
  }

  @Override
  public void game_started(String hunterName, String preyName) {
    
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
            close_conn_game_over("Hunter Timed Out!!!");
            break;

          case 'p':
            close_conn_game_over("You Timed Out!!!");
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

  PreyMoves convertToMove(char[] move) {
    if (move.length < 2) {
      return PreyMoves.ZZ;
    }
    switch (move[0]+move[1]) {
      case 'N'+'N':
        return PreyMoves.NN;

      case 'S'+'S':
        return PreyMoves.SS;

      case 'W'+'W':
        return PreyMoves.WW;

      case 'E'+'E':
        return PreyMoves.EE;

      case 'N'+'E':
        return PreyMoves.NE;

      case 'N'+'W':
        return PreyMoves.NW;

      case 'S'+'E':
        return PreyMoves.SE;

      case 'S'+'W':
        return PreyMoves.SW;

      case 'Z'+'Z':
        return PreyMoves.ZZ;

      default:
        return PreyMoves.ZZ;
    }
  }
}

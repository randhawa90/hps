import java.awt.geom.Point2D;
import java.io.IOException;

public interface EvasionListener {
  
  public void hunter_moved(HunterMove move, String message) throws IOException;
  public void prey_moved(Point2D move) throws IOException;
  public void prey_caught(long no_of_moves) throws IOException;
  public void game_started(String hunterName, String preyName) throws IOException;
  public void game_reset();
  public void time_over(long no_of_moves, char player) throws IOException;
  public String getName();
}

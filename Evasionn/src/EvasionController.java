import java.io.IOException;

import javax.swing.SwingWorker;
import javax.swing.UnsupportedLookAndFeelException;


public class EvasionController {

  public static void main(String[] args) throws ClassNotFoundException, InstantiationException, IllegalAccessException, UnsupportedLookAndFeelException, IOException {
    int N = Integer.parseInt(args[0]);
    int W = Integer.parseInt(args[1]);
    String spec = (args[2]);
    long waitMillis = Long.parseLong(args[args.length-1]);
    final EvasionModel model = new EvasionModel(N, W, waitMillis);
    final EvasionView view = new EvasionView(model, N, W);
    model.register(view, 'v');
    final Hunter hunter;
    final Prey prey;
    RandomHunter randHunt;
    RandomPrey randPrey;
    long huntTime;
    long preyTime;
    int huntPort;
    int preyPort;
    switch (spec.charAt(0)) {
      case 'B':
        huntTime = Integer.parseInt(args[3]);
        preyTime = Integer.parseInt(args[4]);
        huntPort = Integer.parseInt(args[5]);
        preyPort = Integer.parseInt(args[6]);
        hunter = new Hunter(model, N, W, huntTime*1000000000, huntPort);
        prey = new Prey(model, N, W, preyTime*1000000000, preyPort);
        model.register(hunter, 'h');
        model.register(prey, 'p');
        new SwingWorker<Integer, Integer>() {
          @Override
          protected Integer doInBackground() throws Exception {
            hunter.init_connection();
            model.hunterReady();
            return 0;
          }
        }.execute();
        new SwingWorker<Integer, Integer>() {
          @Override
          protected Integer doInBackground() throws Exception {
            prey.init_connection();
            model.preyReady();
            return 0;
          }
        }.execute();
        break;

      case 'H':
        huntTime = Integer.parseInt(args[3]);
        huntPort = Integer.parseInt(args[4]);
        hunter = new Hunter(model, N, W, huntTime*1000000000, huntPort);
        randPrey = new RandomPrey(model, N, W);
        model.register(hunter, 'h');
        model.register(randPrey, 'p');
        model.preyReady();
        hunter.init_connection();
        model.hunterReady();
        break;
        
      case 'P':
        preyTime = Integer.parseInt(args[3]);
        preyPort = Integer.parseInt(args[4]);
        randHunt = new RandomHunter(model, N, W);
        prey = new Prey(model, N, W, preyTime*1000000000, preyPort);
        model.register(randHunt, 'h');
        model.register(prey, 'p');
        model.hunterReady();
        prey.init_connection();
        model.preyReady();
        break;
        
      case 'N':
      default:
        randHunt = new RandomHunter(model, N, W);
        randPrey = new RandomPrey(model, N, W);
        model.register(randHunt, 'h');
        model.register(randPrey, 'p');
        model.hunterReady();
        model.preyReady();
        break;
    }
    
  }

}

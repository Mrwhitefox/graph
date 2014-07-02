This package follow the MVC (Model View Controler) pattern:

1) The data (nodes and links) is stored in SimulationModel.java.

2) The thread where the physic simulation is computed is in SimulationControler.java.

3) An implementation of the view using JPanel is disponible in SimulationViewJPanel.java.
   If you want to use other library to display the simulation (for example for Android), the view class must implement the interface SimulationView.java.

 
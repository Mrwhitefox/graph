package com.github.vmarquet.graphJavaPackage.test;

import com.github.vmarquet.graphJavaPackage.model.SimulationModel;
import com.github.vmarquet.graphJavaPackage.model.Node;
import com.github.vmarquet.graphJavaPackage.model.Link;
import com.github.vmarquet.graphJavaPackage.view.SimulationView;
import com.github.vmarquet.graphJavaPackage.view.SimulationViewJPanel;
import com.github.vmarquet.graphJavaPackage.controler.GraphGeneratorRandom;
import com.github.vmarquet.graphJavaPackage.controler.SimulationControler;
import javax.swing.JFrame;
import java.lang.Thread;
import javax.swing.WindowConstants;

public class Test100 {

	public static void main(String[] args) {
		
		// on remplit le modèle
		GraphGeneratorRandom generator = new GraphGeneratorRandom(100);

		SimulationModel model = SimulationModel.getInstance();
		model.print();

		// on crée une vue pour la simulation
		SimulationViewJPanel panel = new SimulationViewJPanel();
		JFrame fen = new JFrame();
		fen.setSize(640,480);
		//fen.setResizable(false);
		fen.add(panel);
		fen.setVisible(true);
		fen.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		panel.requestFocus();

		// on lance la simulation
		Thread thread = new Thread(new SimulationControler(panel));
		thread.start();

		return;
	}

}

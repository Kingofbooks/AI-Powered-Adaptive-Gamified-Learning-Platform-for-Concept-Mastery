import { useState, useCallback } from 'react';
import { GameContext } from './gameContextSetup';

export default function GameProvider({ children }) {
  const [selectedSubject, setSelectedSubject] = useState(null);
  const [selectedConcept, setSelectedConcept] = useState(null);
  const [currentGame, setCurrentGame] = useState(null);
  const [gameLoading, setGameLoading] = useState(false);

  const selectSubject = useCallback((subject) => {
    setSelectedSubject(subject);
    setSelectedConcept(null);
    setCurrentGame(null);
  }, []);

  const selectConcept = useCallback((concept) => {
    setSelectedConcept(concept);
    setCurrentGame(null);
  }, []);

  const startGame = useCallback((game) => {
    setGameLoading(true);
    setTimeout(() => {
      setCurrentGame(game);
      setGameLoading(false);
    }, 500);
  }, []);

  const resetGame = useCallback(() => {
    setCurrentGame(null);
    setGameLoading(false);
  }, []);

  const value = {
    selectedSubject,
    selectedConcept,
    currentGame,
    gameLoading,
    selectSubject,
    selectConcept,
    startGame,
    resetGame,
  };

  return (
    <GameContext.Provider value={value}>
      {children}
    </GameContext.Provider>
  );
}

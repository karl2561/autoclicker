#!/usr/bin/env bash

SESSION="autoclicker"

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if tmux has-session -t $SESSION 2>/dev/null; then
    tmux attach -t $SESSION
    exit 0
fi

tmux new-session -d -s "$SESSION" -c "$PROJECT_DIR" -n lazygit
tmux send-keys "lazygit" C-m

tmux new-window -t "$SESSION" -c "$PROJECT_DIR" -n terminal

tmux new-window -t "$SESSION" -c "$PROJECT_DIR" -n todo
tmux send-keys "nvim todo.md" C-m

tmux select-window -t "$SESSION:todo"
tmux attach -t "$SESSION"

import torch.nn as nn


# Parameters
LANDMARK_VEC_DIM = 63
HIDDEN_SIZE = 8

# handle the input files

class processing_input():
    


class RNNClassifier(nn.Module):
    def __init__(self, n_classes):
        # Set parameters internally (TODO - 수정 필요)
        self.input_size = LANDMARK_VEC_DIM
        self.hidden_size = HIDDEN_SIZE
        self.n_classes = n_classes

        self.rnn = nn.LSTM(input_size=input_size, hidden_size=hidden_size)
        # LSTM의출력으로부터 클래스를 나누는 레이어
        self.generator = nn.Linear(hidden_size * 2, n_classes)




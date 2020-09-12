import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim

SEED = 5
random.seed(SEED)
torch.manual_seed(SEED)

USE_CUDA = torch.cuda.is_available()
DEVICE = torch.device("cuda" if USE_CUDA else "cpu")


lstm = nn.LSTM(21, 3)
# inputs = 

class LSTMClassifier(nn.Module):
    def __init__(self, embedding_dim, hidden_dim, vocab_size, tagset_size):
        # vocab_size 는 정의되지 않을 예정이다.. 어쩌지


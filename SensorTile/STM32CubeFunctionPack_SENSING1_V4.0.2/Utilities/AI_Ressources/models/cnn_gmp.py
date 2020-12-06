""" Example of conversion module for Lasagne """
import theano.tensor as T
import lasagne.init as li
import lasagne.layers as ll
import lasagne.nonlinearities as ln


INPUT = T.tensor4('inputs')
INPUT_SHAPE = (None, 1, 24, 3)


def network(input_var):
    """ Builds a CNN with 2 convolutional layers and global max pooling.
    The sensor channels are processed separately up to the global pooling
    layer."""
    win_size = 24
    n_inputs = 3
    num_classes = 5
    num_filters = 16
    filter_size = 5

    net = ll.InputLayer(
        shape=(None, 1, win_size, n_inputs), input_var=input_var)
    net = ll.batch_norm(ll.Conv2DLayer(
        ll.batch_norm(net), num_filters, (filter_size, 1),
        W=li.GlorotUniform()))
    net = ll.Conv2DLayer(net, num_filters, (filter_size, 1),
                         W=li.GlorotUniform())
    net = ll.GlobalPoolLayer(net, pool_function=T.max)
    net = ll.DenseLayer(ll.dropout(net, p=.5),
                        num_units=num_classes, nonlinearity=ln.softmax)

    return net


if __name__ == '__main__':
    pass

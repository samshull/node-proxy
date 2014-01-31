{
  'targets': [
    {
      'target_name': 'nodeproxy',
      'sources': [
        'src/node-proxy.cc',
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
    }
  ]
}
